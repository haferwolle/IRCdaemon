from sqlite3 import dbapi2 as sqlite
connection=sqlite.connect('logging.db')
cursor=connection.cursor()
cursor.execute('SELECT * FROM activity')
for row in cursor:
	print 'id:',row[0]
	print 'user:',row[1]
	print 'channel:',row[2]
	print 'time:',row[4]
	print 'message:',row[3]
	print '\n'
	
cursor.close()
connection.close()
