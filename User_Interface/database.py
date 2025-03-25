import os, sys
import pymysql as mdb
from bottle import FormsDict
from hashlib import md5


# Connect to MySQL database
def connect():
    return mdb.connect(
        host="localhost",
        user="kyt3",
        password="password",
        database="ece445_project"
    )

# Fetch user by username to see if the user exists
def fetchUser(username):
    conn = connect()
    cursor = conn.cursor()
    
    query = 'select username from users where username = %s'
    cursor.execute(query, (username,))

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True
    

# Validate user login (to see if account creation worked)
def validateUser(username, password):
    conn = connect()
    cursor = conn.cursor()
    password_hash = str(md5(password.encode('utf-8')).hexdigest())

    query = "select * from users where username = %s and passwordhash = %s"
    cursor.execute(query, (username, password_hash))  # Assume passwordhash is stored
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

# given the username, get the userID
def getUserID(username):
    conn = connect()
    cursor = conn.cursor()
    
    query = 'select userID from users where username = %s'
    cursor.execute(query, (username,))

    userID = cursor.fetchone()

    cursor.close()
    conn.close()
    return userID

# get the petID given the user and the petname from the session
def getPetID(username,petname):
    conn = connect()
    cursor = conn.cursor()
    
    userID = getUserID(username)

    query = 'select petID from pets where userID = %s and petName = %s'
    cursor.execute(query, (userID,petname))

    petID = cursor.fetchone()

    cursor.close()
    conn.close()

    return petID

# Create new pet for a user
def createPet(username, petname, rfid, petdescription, petfood):
    conn = connect()
    cursor = conn.cursor()
    
    userID = getUserID(username)
    
    query = 'insert into pets (userID, rfID, petName, petDescription, petfood) values (%s,%s,%s,%s,%s)'
    cursor.execute(query, (userID, rfid, petname,petdescription,petfood))

    conn.commit()
    cursor.close()
    conn.close()

# create a feeding time
def createFeedingTime(username, petname, feeding_time, amount):
    conn = connect()
    cursor = conn.cursor()
    
    petID = getPetID(username, petname)
    
    query = 'insert into feeding_times (petID, feeding_time, amount) values (%s,%s,%s)'
    cursor.execute(query, (petID, feeding_time, amount))

    conn.commit()
    cursor.close()
    conn.close()

# singular for verification of feeding time creation
def getFeedingTime(username,petname, feeding_time):
    conn = connect()
    cursor = conn.cursor()

    petID = getPetID(username,petname)

    query = "select * from feeding_times where petID = %s and feeding_time = %s"
    cursor.execute(query, (petID,feeding_time))

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True


# get the single pet for verification to make sure the pet was created correctly
def getPet(userID, petname):
    conn = connect()
    cursor = conn.cursor()

    query = "select * from pets where userID = %s and petname = %s"
    cursor.execute(query, (userID,petname))
    # pets = cursor.fetchall()

    if cursor.rowcount < 1:
        return False
    cursor.close()
    conn.close()
    return True
    # cursor.close()
    # conn.close()
    # return pets

# get the pets for a given user
def getPets(username):
    conn = connect()
    cursor = conn.cursor()
    userID = getUserID(username)

    query = "select * from pets where userID = %s"
    cursor.execute(query, (userID,))
    pets = cursor.fetchall()
    cursor.close()
    conn.close()
    return pets

# delete the pet 
def deletePet(username, petname):
    conn = connect()
    cursor = conn.cursor()
    userID = getUserID(username)

    query = "delete from pets where userID = %s and petname = %s"
    cursor.execute(query, (userID,petname))

    conn.commit()
    cursor.close()
    conn.close()

# Fetch feeding times for a pet
def getFeedingTimes(username):
    conn = connect()
    cursor = conn.cursor()
    userID = getUserID(username)

    query = """
        select petID, feeding_time, amount from feeding_times
        where petID in (select petID from pets where userID = %s)
    """
    cursor.execute(query, (userID,))
    feeding_times = cursor.fetchall()
    cursor.close()
    conn.close()
    return feeding_times

# Fetch pet details
def getPetDetails(petID, userID):
    conn = connect()
    cursor = conn.cursor()

    query = "select * from pets where petID = %s and userID = %s"
    cursor.execute(query, (petID, userID))

    pet = cursor.fetchone()
    
    cursor.close()
    conn.close()
    return pet

# Fetch feeding history for a pet
def getFeedingHistory(petID):
    conn = connect()
    cursor = conn.cursor()

    query = "select * from history where petID = %s limit 15"
    cursor.execute(query, (petID,))

    history = cursor.fetchall()

    cursor.close()
    conn.close()
    return history


# Fetch feeding history for a pet
def getHistory(username):
    conn = connect()
    cursor = conn.cursor()

    userID = getUserID(username)
    query = ''' 
                select petID, status_update, event_time from history 
                where petID in (select petID from pets where userID  = %s) limit 15
            '''
    cursor.execute(query, (userID,))

    history = cursor.fetchall()
    cursor.close()
    conn.close()
    return history

# this will create a history entry and add it to the database
def createHistoryEntry(username, petname, data, time):
    conn = connect()
    cursor = conn.cursor()
    
    petID = getPetID(username, petname)
    
    query = 'insert into history (petID, status_update, event_time) values (%s,%s,%s)'
    cursor.execute(query, (petID, data, time))

    conn.commit()
    cursor.close()
    conn.close()
