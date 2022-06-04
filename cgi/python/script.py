import cgi, cgitb, time

form = cgi.FieldStorage() 

fname = form.getvalue('fname')
lname = form.getvalue('lname')

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2>" % (fname, lname))
print ("</body>")
print ("</html>")