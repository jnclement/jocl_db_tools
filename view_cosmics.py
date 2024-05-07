import cgi
import psycopg2
import sys
import argparse
from datetime import datetime, date

parser = argparse.ArgumentParser(description='get runrange')
parser.add_argument('--first',action="store",dest='first',default=-1)
parser.add_argument('--last',action="store",dest='last',default=-1)
parser.add_argument('--time',action="store",dest='time',default=-1)
parser.add_argument('--vgtm',action="store",dest='vgtm',nargs='*',default=-1)
args = parser.parse_args()
firstrun = args.first
lastrun = args.last
pasttime = args.time
vgtm = args.vgtm

if firstrun > lastrun:
    print("lastrun can't be greater than firstrun!")
    quit()


db_params = {
        'dbname': 'daq',
        'user': 'phnxro',
        'host': 'db1.sphenix.bnl.gov'
        }

conn = psycopg2.connect(**db_params)
cursor = conn.cursor()
hists = []

f = open('/home/phnxrc/samfred/db/cosmics.txt','w')
def write_entry(entry):
    sentry = str(entry)
    sentry = sentry.replace('(','').replace(')','').replace(',','').replace('datetime.datetime','')
    a = sentry.split(' ')
    try:
        a = [int(val) for val in a]
    except:
        print("none value found")
        return
    starttime = datetime(a[3],a[4],a[5],a[6],a[7],a[8])
    endtime = datetime(a[9],a[10],a[11],a[12],a[13],a[14])
    runtime = (endtime-starttime).total_seconds()
    sentry += " " + str(runtime)
    f.write(sentry+'\n')

clauseword = "WHERE"
# join relevant tables
command = "SELECT S.runnumber,S.index,S.raw,R.brtimestamp,R.ertimestamp FROM gl1_scalers S INNER JOIN run R ON S.runnumber = R.runnumber"
# add a table and include vgtm search
if vgtm != -1:
    command += " INNER JOIN gtm G on S.runnumber = G.runnumber WHERE G.vgtm = " + vgtm[0] + " and G.global = 't'"
    clauseword = "AND"

#search via time
if pasttime != -1:
    currentdate = str(date.today())
    command += " " + clauseword + " date \'" + currentdate + "\' - S.updated::date < " + pasttime
#search via runnumbers
else:
    command += " " + clauseword + " S.runnumber <= " + lastrun + " and S.runnumber >= " + firstrun

#runnumber check just to make sure we're getting real runs
command += " and R.runtype = \'cosmics\' and S.runnumber > 10000 ORDER BY S.runnumber,S.index"

cursor.execute(command)
for entry in cursor.fetchall():
    write_entry(entry)

cursor.close()
conn.close()
f.close()
