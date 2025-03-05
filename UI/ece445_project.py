#!/usr/bin/env python3
import sys, os, re, bottle
from bottle import template, route, Bottle
os.chdir(os.path.dirname(os.path.abspath(__file__)))

from bottle import get, post, run, debug, request, response, redirect, view, FormsDict, HTTPError
import database

# Cookie tracking for user login
authSecret = "2c1ef059cbacacd273878663816e072d381c98a37fb46a2bcdd93aedf80bac10"

# app = Bottle()

# Home page - display user information and feeding times
@get("/")
@view("index")
def index():
    username = request.forms.get("username")  # Or session management, or any other method you have for the username
    # if not username:
    #     return HTTPError(403, "You must be logged in")

    user = database.fetchUser(username)  # Now passing the username to fetchUser()
    if user:
        pets = database.getPets(user.userID)  # Fetch pets from the database
        feeding_times = database.getFeedingTimes(user.userID)  # Fetch feeding times for the pets
    else:
        pets = []
        feeding_times = []

    return dict(user=user, pets=pets, feeding_times=feeding_times)
    # return user

# Pet details page - show details about the pet and feeding history
@get("/pet")
@view("pet_detail")
def pet_detail(petID):
    username = request.forms.get("username")  # Or session management, or any other method you have for the username

    user = database.fetchUser(username)
    pet = database.getPetDetails(petID, user.userID)  # Fetch pet details
    if pet:
        feeding_history = database.getFeedingHistory(petID)
    else:
        feeding_history = []
    return dict(user=user, pet=pet, feeding_history=feeding_history)

@get("/user_page")
@view("user_page")
# @route("user_page")
def user_page(user = None):
    user = getUser()
    # username = request.forms.get("username")  # Or session management, or any other method you have for the username
    print(f"Username received: {{user}}")  # Debugging line
    # user = database.fetchUser(username)
    # pet = database.getPetDetails(petID, user.userID)  # Fetch pet details
    # if pet:
        # feeding_history = database.getFeedingHistory(petID)
    # else:
    #     feeding_history = []
    # return dict(user=user)  #, pet=pet, feeding_history=feeding_history)
    return template("user_page", user)

# Login page - handle login authentication
@get("/login")
@route("login")
def login(create=False):
    print(f'in login and create is {create}')
    username = request.query.get("username")
    password = request.query.get("password")
    print(f'username is {username} and pw is {password}')
    if username == None or password == None:
        raise HTTPError(400, "Required field is empty")
    
    # User authentication using database methods
    if create:
        if database.fetchUser(username):
            raise HTTPError(400, "User already exists")
        if len(password) < 4:
            raise HTTPError(400, "Password too short")
        database.createUser(username, password)

    if not database.validateUser(username, password):
        raise HTTPError(403, "Login unsuccessful")
    response.set_cookie("authuser", username, authSecret, httponly=True)
    
    # redirect("./user_page")
    # redirect("/user_page?user={username}")
    # user_page(user=username)
    return template("./user_page", user=username)
    # return template("/user_page")

# Logout page - handle user logout
@post("/logout")
def logout():
    response.delete_cookie("authuser")
    
    redirect("./")

# User session helper function
# def getUser():
    # username = request.get_cookie("authuser", None, secret=authSecret)
    # if username is None:
    #     return None
    # return database.fetchUser(username)

# For creating new user (optional)
@get("/create")
@route("create")
def create():
    login(create=True)

def getUser():
    username = request.get_cookie("authuser", None, secret=authSecret)
    if username is None:
        return None
    return database.fetchUser(username)

# Database methods need to be implemented in 'database.py'
# For example:
# - database.getPets(userID)
# - database.getFeedingTimes(userID)
# - database.getPetDetails(petID, userID)
# - database.getFeedingHistory(petID)
# - database.createUser(username, password)
# - database.fetchUser(username)
# - database.validateUser(username, password)

if __name__ == "__main__":
    debug(True)
    run(host='127.0.0.1', reloader=True)
