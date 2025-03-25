from bottle import Bottle, run, template, request, redirect, HTTPError
from beaker.middleware import SessionMiddleware
import database

# Create Bottle app
app = Bottle()  

# Configure session to be able to store information
session_opts = {
    'session.type': 'memory',
    'session.auto': True
}

# will remember past sessions (i.e. will remember the user as you move pages)
wrapped_app = SessionMiddleware(app, session_opts)  

# this will display the login page
@app.route('/')
def login():
    return template('views/login.html')
    
# this POST request will create a new user if the Create New Account button is clicked
@app.route("/create", method='POST')
def create():
    do_login(create=True)

# this POST request will log in an existing user if the login button is clicked
@app.route('/login', method='POST')
def do_login(create=False):
    session = request.environ['beaker.session']
    username = request.forms.get('username')
    password = request.forms.get('password')

    if username == None or password == None:
        raise HTTPError(400, "Required field is empty")
    
    if create:
        if database.fetchUser(username):
            raise HTTPError(400, "User already exists")
        if len(password) < 4:
            raise HTTPError(400, "Password too short")
        database.createUser(username, password)

    if not database.validateUser(username, password):
        raise HTTPError(403, "Login unsuccessful")
    
    # set the user as the username of the person logging in
    session['user'] = username
    session.save()
    redirect('/dashboard')

# this POST request will delete a pet if the delete pet button is clicked and refresh the page
@app.route('/deletepet', method='POST')
def deletepet():
    session = request.environ['beaker.session']
    petname = request.forms.get("petName")

    if 'user' in session:
        print(f'deleting pet {petname}')
        database.deletePet(session['user'], petname)

    session.save()
    redirect('/dashboard')

# this will allow the user to create a new pet and will refresh the page
@app.route('/createPet', method='POST')
def createPet():
    session = request.environ['beaker.session']
    petname = request.forms.get('petName')
    rfID = request.forms.get('rfID_UIN')
    petdescription =request.forms.get('petDescription')
    petfood =request.forms.get('petfood')

    if petname == '' or rfID == '' or petdescription == '' or petfood =='':
        print("empty field for pet")
        raise HTTPError(400, "Required field is empty")
    
    if 'user' in session:
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

# this POST request will allow the user to create a feeding time for thier pets
@app.route('/createFeedingTime', method='POST')
def createFeedingTime():
    session = request.environ['beaker.session']
    petname = request.forms.get('petName')
    feedingTime = request.forms.get('feeding_time')
    amount =request.forms.get('amount')

    if  petname == '' or feedingTime == ''  or amount == '':
        print("empty field for feeding time")
        raise HTTPError(400, "Required field is empty")
    
    if 'user' in session:
        username = session['user']
        # this operates on the assumption that the user is logged in, and the rfID is correct
        database.createFeedingTime(username, petname, feedingTime, amount)
        
        if not database.getFeedingTime(username,petname, feedingTime):
            raise HTTPError(403, "Creation of Feeding Time was unsuccessful")
        else:
            print("Feeding time was successfully added to the database")

        session.save()
        return redirect('/dashboard')
    else:
        print("user is not in session")
    return redirect('/login')

# this displays the dashboard template and basic information for the user
@app.route('/dashboard')
def dashboard():
    session = request.environ['beaker.session']
    if 'user' in session:
        pets = database.getPets(session['user'])
        feedingTimes = database.getFeedingTimes(session['user'])
        return template('views/dashboard.html', username=session['user'],pets = pets, feeding_times=feedingTimes)
    else:
        redirect('/')


#TODO: Test this function when we have the ESP32 chip. This is supposed to receive infromation from the chip and push it to the users history
# @app.route('/receive', method='POST')
# def receive():
#     session = request.environ['beaker.session']
#     data = request.body.read().decode('utf-8')

#     # Store received message in session history
#     if 'messages' not in session:
#         session['messages'] = []
    
#     session['messages'].append(data)
#     session.save()  # Save session changes

#     if 'user' in session:
#         petname = "dog"

#         database.createHistoryEntry(session['user'], petname, data, datetime.datetime.now())

#     print(f"Received: {data}")  # Debugging
#     return "Data received"


# this will redirect the user to the history page to see thier pet history and display the history template
@app.route('/history')
def history():
    session = request.environ['beaker.session']
    if 'user' in session:
        pets = database.getPets(session['user'])
        feedingTimes = database.getFeedingTimes(session['user'])
        history = database.getHistory(session['user'])
        return template('views/history.html', username=session['user'],pets = pets, feeding_times=feedingTimes, history=history)
    else:
        redirect('/')


# this reroutes the user the the login page and deletes the current session
@app.route('/logout')
def logout():
    session = request.environ['beaker.session']
    session.delete()
    redirect('/')


# if this file is run, this will call the app and run it on port 8080
if __name__ == "__main__":
    run(app=wrapped_app, host='localhost', port=8080, debug=True, reloader=True)  