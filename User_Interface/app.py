from bottle import Bottle, run, template, request, redirect, HTTPError,static_file, route, response
from beaker.middleware import SessionMiddleware
import database
import json
from datetime import datetime, timedelta
# from wsgiref.simple_server import make_server
# import sys

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
def index():
    print("in the '/' route")
    # return "hello"
    try:
        print('in the try catch for "/"')
        return template('views/login.html')
    except Exception as e:
        print(f"Error rendering template: {e}")
        raise e
    # return template('views/login.html')

@app.route('/test')
def test():
    return "Test route works!"


#this allows static files to be accessed (ex. images)
@app.route('/static/<filepath:path>')
def server_static(filepath):
    return static_file(filepath, root='./static')

    
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


# this will handle all data processing from the PCB
@app.route('/receive', method=['GET','POST'])
def receive_data():
    # print("here")
    # print("== /receive called ==")
    # print("Method:", request.method)
    # print("Headers received:", dict(request.headers))
    # print("Raw body:", request.body.read())
    request.body.seek(0)

    data = request.json
    print("Parsed JSON:", data)

    # if request.method == 'GET':
    #     return "GET is working"
    # if not data:
    #     return {'status': 'error', 'message': 'Invalid or missing JSON'}

    # print("Headers received:", dict(request.headers))
    # print("Raw body:", request.body.read())
    # print("ESP32 Sent:", data)

    if 'tag' in data:
        tag = int(data['tag'],16)
        print(f'printing the type of the tag {type(tag)}')
        print(f'printing the int of the tag UIN {tag}')

        petID = database.getPetbyRFID(tag)
        print(f'the petid returned is {petID}')

        feeding_times = database.getFeedingTimebyPetID(petID)
        print(f"the feeding times type is {type(feeding_times)}")
        print(f"the first feeding times is {feeding_times[0][2]}")
        now = datetime.now().time()
        current_time = timedelta(hours=now.hour, minutes=now.minute, seconds=now.second)

        print(f'the current time is {current_time}')
        print(f"the feeding times are {feeding_times}")

        dispense_food = False
        for feeding_time in feeding_times:
            print(f"the feeding times are {feeding_time[2]}")

            if feeding_time[2] <= current_time <= (feeding_time[2] + timedelta(minutes=30)):
                print(f'dispense food')
                dispense_food = True
                break
            else:
                print(f'need to wait')


        # try:
        #     tag = int(data['tag'],16)
        #     print(f'printing the type of the tag {type(tag)}')
        #     print(f'printing the int of the tag UIN {tag}')

        #     petID = database.getPetbyRFID(tag)

        #     feeding_time = database.getFeedingTimebyPetID(petID)
        #     print(f"the feeding times type is {type(feeding_time)}")
        #     print(f"the feeding times are {feeding_time}")

        # except ValueError:
        #     return {'status': 'error', 'message': 'Invalid tag format'}
        
    # return {'status': 'received'}
    
    return {'status': 'received', "dispense_food": f"{dispense_food}"}

@app.route('/command', method='GET')
def send_command():
    command = {"action": "turn_on_led", "value": 1}
    response.content_type = 'application/json'
    return json.dumps(command)



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

# print("Routes defined:")
# for r in app.routes:
#     print(r.rule)


# if this file is run, this will call the app and run it on port 8080
if __name__ == "__main__":
    # print(f"__name__ is {__name__}")
    print("here")
    # print("Routes defined:")
    # for r in app.routes:
    #     print(r.rule)

    # try:
    #     # server = make_server('localhost', 8080, wrapped_app)
    #     # print("Running on http://localhost:8080/")
        
    #     # server.serve_forever()
    #     run(app=wrapped_app, host='localhost', port=8080, debug=True, reloader=True)
    #     # run(app=wrapped_app, host='localhost', port=8080, debug=True, reloader=True)  

    # except KeyboardInterrupt:
    #     print("\nServer shut down gracefully.")
    #     sys.exit(0)

    run(app=wrapped_app, host='0.0.0.0', port=8080, debug=True, reloader=True)  
