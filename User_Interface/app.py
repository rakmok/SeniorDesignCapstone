from bottle import Bottle, run, template, request, redirect
from beaker.middleware import SessionMiddleware
# from database import connect, fetchUser, getFeedingHistory,getFeedingTimes,getPetDetails,getPets
import database
from bottle import get, post, run, debug, request, response, redirect, view, FormsDict, HTTPError


app = Bottle()  # Create Bottle app

# Configure session
session_opts = {
    'session.type': 'memory',
    'session.auto': True
}

# will remember past sessions (i.e. will remember the user as you move pages)
wrapped_app = SessionMiddleware(app, session_opts)  # Wrap the app AFTER defining routes
authSecret = 'supersecretcookie'

@app.route('/')
def login():
    return template('views/login.html')
    
@app.route("/create", method='POST')
def create():
    do_login(create=True)

@app.route('/login', method='POST')
def do_login(create=False):
    session = request.environ['beaker.session']
    username = request.forms.get('username')
    password = request.forms.get('password')

    print(f'in login and create is {create}')
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
    # response.set_cookie("authuser", username, authSecret, httponly=True)
    
    session['user'] = username
    session.save()
    redirect('/dashboard')

    # return "Invalid username or password. <a href='/'>Try again</a>."

@app.route('/createPet', method='POST')
def createPet():
    session = request.environ['beaker.session']
    petname = request.forms.get('petName')
    rfID = request.forms.get('rfID_UIN')
    petdescription =request.forms.get('petDescription')
    petfood =request.forms.get('petfood')

    print(f'in petname is {petname} and rfid is {rfID}')
    if petname == '' or rfID == '' or petdescription == '' or petfood =='':
        print("empty field for pet")
        raise HTTPError(400, "Required field is empty")
    
    if 'user' in session:
        print("user is logged in and trying to create a pet")

        username = session['user']
        # this operates on the assumption that the user is logged in, and the rfID is correct
        database.createPet(username, petname, rfID, petdescription, petfood)
        userID = database.getUserID(username)
        if not database.getPet(userID, petname):
            raise HTTPError(403, "Pet creation unsuccessful")
        else:
            print("pet was successfully added to the database")
        
        session.save()
        return redirect('/dashboard')

    else:
        print("user is not in session")

    return redirect('/login')


@app.route('/createFeedingTime', method='POST')
def createFeedingTime():
    session = request.environ['beaker.session']
    petname = request.forms.get('petName')
    feedingTime = request.forms.get('feeding_time')
    amount =request.forms.get('amount')

    print(f'in petname is {petname} and feeding time is {feedingTime} aand amount is {amount}')
    if  petname == '' or feedingTime == ''  or amount == '':
        print("empty field for feeding time")
        raise HTTPError(400, "Required field is empty")
    
    if 'user' in session:
        print("user is logged in and trying to create a new feeding time")

        username = session['user']
        # this operates on the assumption that the user is logged in, and the rfID is correct
        database.createFeedingTime(username, petname, feedingTime, amount)
        # userID = database.getUserID(username)
        if not database.getFeedingTime(username,petname, feedingTime):
            raise HTTPError(403, "Creation of Feeding Time was unsuccessful")
        else:
            print("Feeding time was successfully added to the database")
        
        session.save()
        return redirect('/dashboard')

    else:
        print("user is not in session")

    return redirect('/login')


@app.route('/dashboard')
def dashboard():
    session = request.environ['beaker.session']
    if 'user' in session:
        # return f"Welcome, {session['user']}! <a href='/logout'>Logout</a>"
        # print(f'user is {session['user']}')
        print(f"user is {session['user']}")
        pets = database.getPets(session['user'])
        feedingTimes = database.getFeedingTimes(session['user'])
        print(f'the pets are {pets}')
        print(f'the feeding times for {session["user"]} are {feedingTimes}')
        for pet in pets:
            print(f'the pet are {pet[3]}')
            # print(f'the pet are {pet[3]}')

        return template('views/dashboard.html', username=session['user'],pets = pets, feeding_times=feedingTimes)
    else:
        redirect('/')

@app.route('/logout')
def logout():
    session = request.environ['beaker.session']
    session.delete()
    redirect('/')

if __name__ == "__main__":
    # Run Shiny in a separate thread
    # threading.Thread(target=run_shiny).start()
    run(app=wrapped_app, host='localhost', port=8080, debug=True, reloader=True)  # Use wrapped_app here