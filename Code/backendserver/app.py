from slack_bolt.adapter.flask import SlackRequestHandler
from flask import Flask, request
from slack_bolt.app import App
import requests
import datetime
from db import DB
import os

db = DB()


# IP address of your ESP32
esp_ip = "192.168.178.154"
endpoint = f"http://{esp_ip}/get"
channel_id = "C07418ANJDT"

def send_dispense(patient_id):
    return requests.get(endpoint, params={"arg": patient_id})

def current_time():
    now = datetime.datetime.now()
    return now.strftime("%Y-%m-%d %H:%M:%S")

# Initialize your app with your bot token and signing secret
app = App(
    token=os.environ.get("SLACK_BOT_TOKEN"),
    signing_secret=os.environ.get("SLACK_SIGNING_SECRET")
)

# Register a slash command handler
@app.command("/dispense")
def handle_command(ack, respond, command):
    ack()  # Acknowledge the command request
    # Split the command text into parameters
    patient_id = command['text']
    res = send_dispense(patient_id)
    
    if res.status_code == 200:
        # respond(f"Medicine for patient with id {patient_id} has been dispensed!")
        curr_time = current_time()
        db.dispense(patient_id, curr_time)
    else:
        print(f"Error: {res.status_code} - {res.text}")
        respond(f"Medicine for patient {patient_id} could not be dispensed. Please try again.")

# Create a Flask web server
flask_app = Flask(__name__)
handler = SlackRequestHandler(app)

@flask_app.route("/dispense", methods=["POST"])
def slack_events():
    return handler.handle(request)

@flask_app.route('/receive', methods=['GET'])
def receive():
    message = request.args.get('message')
    if message:
        response_message = f"Received message: {message}"
        print(response_message)
        values = message.split('/')
        success = values[0]
        patient = values[1]
        # Send the message to slack
        if (success == "success"):
            message = f"Medicine for patient {patient} has been dispensed!"
        else:
            message = f"Missed dose for {patient}!"
        app.client.chat_postMessage(channel=channel_id, text=message)
        return response_message, 200
    else:
        return "No message received", 400

if __name__ == "__main__":
    flask_app.run(port=3000)