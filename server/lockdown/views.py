from django.shortcuts import render, redirect
from django.http import HttpResponse
from django.contrib.auth import login, authenticate, logout
from django.contrib.auth.forms import UserCreationForm
from django.views.decorators.csrf import csrf_exempt
from django.conf import settings

import json, tempfile, shutil
from os import path, system

from .forms import LoginForm


def index(request):
    if request.user.is_authenticated:
        return redirect('playground')
    else:
        return redirect('signup')


def signup(request):
    if request.method == 'POST':
        form = UserCreationForm(request.POST)
        if form.is_valid():
            form.save()
            return redirect('user_login')
    else:
        form = UserCreationForm()
    return render(request, 'signup.html', {'form': form})


def user_login(request):
    if request.user.is_authenticated:
            return redirect('index')
    if request.method == 'POST':
        username = request.POST['username']
        password = request.POST['password']
        user = authenticate(username=username, password=password)
        if user is not None:
            login(request, user)
            return redirect('playground')
        else:
            return HttpResponse("invalid login")
    else:
        form = LoginForm()
        return render(request, 'login.html', {'form':form})

def user_logout(request):
    if request.user.is_authenticated:
        logout(request)
        return HttpResponse("logged out")
    else:
        return redirect('index')

def playground(request):
    if not request.user.is_authenticated:
        return redirect('user_login')
    else:
        if request.method == 'GET':
            return render(request, 'playground.html')

@csrf_exempt
def runcode(request):
    if (not request.user.is_authenticated) or request.method == 'GET':
        return HttpResponse('Unauthorized', status=401)
    else:
        received_data = json.loads(request.body)
        maketempdir(request.user.username)
        createfile('source', received_data['code'], request.user.username, received_data['lang'])
        createfile('input', received_data['input'], request.user.username)
        tmp_user_dir = '/tmp/' + request.user.username + '/sandbox/'
        source_file = tmp_user_dir + 'source.' + received_data['lang']
        input_file = tmp_user_dir + 'input.txt'
        command = tmp_user_dir + './runme.sh ' + source_file + ' ' + input_file
        system(command)
        return HttpResponse(readoutput(request.user.username))


def maketempdir(username):
    main_dir = path.join(path.dirname(settings.BASE_DIR),settings.SANDBOX_DIR)
    tmp_user_dir = '/tmp/' + username + '/sandbox'
    if not path.exists(tmp_user_dir):
        shutil.copytree(main_dir,tmp_user_dir)

def createfile(fname, content, username, ext='txt'):
    tmp_user_dir = '/tmp/' + username + '/sandbox/'
    f = open(tmp_user_dir + fname + '.' + ext , 'w')
    f.write(content)
    f.close()

def readoutput(username):
    outfile = '/tmp/' + username + '/sandbox/' + 'outfile'
    with  open(outfile,'r') as f:
        return f.read()
    
    
