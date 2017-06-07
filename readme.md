
# About:

[lockdown](https://github.com/Ahiknsr/lockdown) allows you to run c and c++ code in a sandbox.


![playground](https://i.imgur.com/g185wHY.png "playground")

# Installation

before you run the following commands make sure python2 and pip2 are installed in your system  .

`git clone https://github.com/Ahiknsr/lockdown`  
`cd lockdown/server/`  
`pip2 install -r requirements.txt`  
`python2 manage.py migrate`  
`python2 manage.py runserver`  

# Usage

Once the server is running , goto http://127.0.0.1:8000/lockdown/signup/ and create a new user account.   
goto http://127.0.0.1:8000/lockdown/login/ and login using the user account you created in previous step.  
on successful login you will be redirected to http://127.0.0.1:8000/lockdown/playground/ where you can execute the code.

# Architecture

lockdown limits the program access to filesystem, network , memory and terminates the program if it runs for more than 5 seconds.
