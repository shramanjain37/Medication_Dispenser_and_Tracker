import sqlite3
import db_data

DB_NAME = 'dispenser.db'

class DB():
	def __init__(self):
		self.db = sqlite3.connect(DB_NAME, check_same_thread=False)
		self.cursor = self.db.cursor()
		self.cursor.execute(db_data.CREATE_DISPENSES)
		# Create patients data
		self.cursor.execute(db_data.CREATE_PATIENTS)
		self.db.commit()

	def reset(self):
		self.cursor.execute(db_data.DROP_DISPENSES)
		self.cursor.execute(db_data.CREATE_DISPENSES)
		self.cursor.execute(db_data.DROP_PATIENTS)
		self.cursor.execute(db_data.CREATE_PATIENTS)
		for patient in db_data.PATIENS:
			self.cursor.execute(db_data.INSERT_PATIENT, patient)
		self.db.commit()

	def dispense(self, patient_id, time):
		self.cursor.execute(db_data.INSERT_DISPENSE, (patient_id, time))
		self.db.commit()

	def select(self):
		self.cursor.execute('''
			SELECT * FROM dispenses
		''')
		return self.cursor.fetchall()

	def delete(self, id):
		self.cursor.execute('''
			DELETE FROM dispenses
			WHERE id = ?
		''', (id,))
		self.db.commit()

	def clear(self):
		self.cursor.execute('''
			DELETE FROM dispenses
		''')
		# Remove dispenses table
		self.cursor.execute(db_data.DROP_DISPENSES)
		self.db.commit()