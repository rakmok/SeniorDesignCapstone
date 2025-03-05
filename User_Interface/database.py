import os, sys
import pymysql as mdb
from bottle import FormsDict
from hashlib import md5


# Connect to MySQL
def connect():
    return mdb.connect(
        host="localhost",
        user="kyt3",
        password="password",
        database="ece445_project"
    )

# Fetch user by username
def fetchUser(username):
    conn = connect()
    cursor = conn.cursor()
    # password_hash = str(md5(password.encode('utf-8')).hexdigest())

    # query = 'select * from users where username = %s and password = %s'
    # cursor.execute(query, (username, password_hash))
    query = 'select username from users where username = %s'
    cursor.execute(query, (username,))

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True
    
    # return FormsDict(cursor.fetchone())


# Validate user login
def validateUser(username, password):
    conn = connect()
    cursor = conn.cursor()
    password_hash = str(md5(password.encode('utf-8')).hexdigest())

    cursor.execute("SELECT * FROM users WHERE username = %s AND passwordhash = %s", (username, password_hash))  # Assume passwordhash is stored
    user = cursor.fetchone()

    cursor.close()
    conn.close()
    return user

# Create new user
def createUser(username, password):
    conn = connect()
    cursor = conn.cursor()
    password_hash = str(md5(password.encode('utf-8')).hexdigest())
    
    query = 'insert into users (username, passwordhash) values (%s,%s)'
    cursor.execute(query, (username, password_hash))

    conn.commit()
    cursor.close()
    conn.close()

def getUserID(username):
    conn = connect()
    cursor = conn.cursor()
    # password_hash = str(md5(password.encode('utf-8')).hexdigest())

    # query = 'select * from users where username = %s and password = %s'
    # cursor.execute(query, (username, password_hash))
    query = 'select userID from users where username = %s'
    cursor.execute(query, (username,))

    userID = cursor.fetchone()

    cursor.close()
    conn.close()

    return userID

def getPetID(username,petname):
    conn = connect()
    cursor = conn.cursor()
    # password_hash = str(md5(password.encode('utf-8')).hexdigest())

    # query = 'select * from users where username = %s and password = %s'
    # cursor.execute(query, (username, password_hash))
    userID = getUserID(username)


    query = 'select petID from pets where userID = %s and petName = %s'
    cursor.execute(query, (userID,petname))

    petID = cursor.fetchone()

    cursor.close()
    conn.close()

    return petID

# Create new pet
def createPet(username, petname, rfid, petdescription, petfood):
    conn = connect()
    cursor = conn.cursor()
    
    userID = getUserID(username)
    
    query = 'insert into pets (userID, rfID, petName, petDescription, petfood) values (%s,%s,%s,%s,%s)'
    cursor.execute(query, (userID, rfid, petname,petdescription,petfood))

    conn.commit()
    cursor.close()
    conn.close()

def createFeedingTime(username, petname, feeding_time, amount):
    conn = connect()
    cursor = conn.cursor()
    
    # userID = getUserID(username)
    petID = getPetID(username, petname)
    
    query = 'insert into feeding_times (petID, feeding_time, amount) values (%s,%s,%s)'
    cursor.execute(query, (petID, feeding_time, amount))

    conn.commit()
    cursor.close()
    conn.close()

#singular for verification
def getFeedingTime(username,petname, feeding_time):
    conn = connect()
    cursor = conn.cursor()

    petID = getPetID(username,petname)
    cursor.execute("SELECT * FROM feeding_times WHERE petID = %s and feeding_time = %s", (petID,feeding_time))
    # pets = cursor.fetchall()

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True


# Fetch a single pet for verification
def getPet(userID, petname):
    conn = connect()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM pets WHERE userID = %s and petname = %s", (userID,petname))
    # pets = cursor.fetchall()

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True
    # cursor.close()
    # conn.close()
    # return pets

# Fetch pets for a user
def getPets(username):
    conn = connect()
    cursor = conn.cursor()
    userID = getUserID(username)
    cursor.execute("SELECT * FROM pets WHERE userID = %s", (userID,))
    pets = cursor.fetchall()
    cursor.close()
    conn.close()
    return pets

# Fetch feeding times for a pet
def getFeedingTimes(username):
    conn = connect()
    cursor = conn.cursor()
    userID = getUserID(username)

    cursor.execute("""
        SELECT petID, feeding_time, amount FROM feeding_times
        WHERE petID IN (SELECT petID FROM pets WHERE userID = %s)
    """, (userID,))
    feeding_times = cursor.fetchall()
    cursor.close()
    conn.close()
    return feeding_times

# Fetch pet details
def getPetDetails(petID, userID):
    conn = connect()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("""
        SELECT * FROM pets WHERE petID = %s AND userID = %s
    """, (petID, userID))
    pet = cursor.fetchone()
    cursor.close()
    conn.close()
    return pet

# Fetch feeding history for a pet
def getFeedingHistory(petID):
    conn = connect()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT * FROM history WHERE petID = %s", (petID,))
    history = cursor.fetchall()
    cursor.close()
    conn.close()
    return history
