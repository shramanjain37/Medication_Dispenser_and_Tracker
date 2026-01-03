CREATE_DISPENSES = '''
	CREATE TABLE IF NOT EXISTS dispenses(
		id INTEGER PRIMARY KEY,
		patient_id INTEGER,
		time TEXT
	)
'''

CREATE_PATIENTS = '''
	CREATE TABLE IF NOT EXISTS patients(
		id INTEGER PRIMARY KEY,
		name TEXT,
		age INTEGER,
		weight INTEGER,
		height INTEGER
	)
'''

DROP_DISPENSES = '''
	DROP TABLE IF EXISTS dispenses
'''

DROP_PATIENTS = '''
	DROP TABLE IF EXISTS patients
'''

INSERT_PATIENT = '''
	INSERT INTO patients(name, age, weight, height)
	VALUES(?, ?, ?, ?)
'''

INSERT_DISPENSE = '''
	INSERT INTO dispenses(patient_id, time)
	VALUES(?, ?)
'''

PATIENS = [
	('John Doe', 25, 70, 170),
	('Jane Doe', 30, 60, 160),
	('John Smith', 40, 80, 180),
	('Jane Smith', 35, 65, 165),
	('John Brown', 45, 85, 175),
	('Jane Brown', 50, 55, 155)
]