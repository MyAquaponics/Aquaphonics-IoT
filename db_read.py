import pymysql

conn=pymysql.connect(host="localhost", user="root", passwd="vision", db="status_db")

try :
    with conn.cursor() as cur :
        sql="select * from status_tb"
        cur.execute(sql)
        for row in cur.fetchall() :
            print(row[0],row[1],row[2])
finally :
    conn.close()
