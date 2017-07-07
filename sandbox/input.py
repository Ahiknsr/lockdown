import os
import pwd
import urllib2

print "hello sandbox"
print os.getuid()
print os.getgid()
response = urllib2.urlopen('http://python.org/')
html = response.read()
print html
