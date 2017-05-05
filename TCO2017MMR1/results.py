import sys
from bs4 import BeautifulSoup
from urllib.request import urlopen
import requests
import re
import browser_cookie3
from pycookiecheat import chrome_cookies
import time

print('username:', end='')
username = input()
print('password:', end='')
password = input()
print('Whose Score:', end='')
who = input()

competitors = '0'

tcc = 'https://community.topcoder.com'
contest = tcc + '/longcontest/stats/?rd=16903'
r = requests.get(contest+'&module=ViewOverview')
for overview in BeautifulSoup(r.text.encode(r.encoding), 'html.parser').find_all('span', class_='bodySubtitle'):
    r = re.search(r'.*?Competitors: *([0-9]*).*?', overview.text)
    if r:
        competitors = r.group(1)
        
if competitors == '0':
    print('No user competes')
    sys.exit(0)

r = requests.get(contest+'&module=ViewOverview'+'&nr='+competitors)
ranking = BeautifulSoup(r.text.encode(r.encoding), 'html.parser')

user = ranking.find('td', string=who)
if user == None:
    print('I Can\'t find this user')
    sys.exit(0)

rank = user.parent.find('td', class_='valueC').string

print('rank:' + str(rank))

s = requests.Session()
data = {
    'nextpage':'https://community.topcoder.com/longcontest/',
    'module':'Login',
    'ha':username,
    'pass':password,
    'rem':'off'
}
s.post('https://community.topcoder.com/longcontest/', data=data)

with open(str(who)+'_result.txt', 'w') as f:
    i = 0
    cnt = 1000
    while i < cnt:
        url = contest+'&module=ViewSystemTestResults'+'&pm=14565'+'&sr='+rank+'&stc='+str(i+1)
        r = s.get(url)
        soup = BeautifulSoup(r.text.encode(r.encoding), 'html.parser')

        details = soup.find_all('span', style='font-weight: normal;')
        scores = soup.find('tr', align='right').find_all('td')
        x = i
        for j in range(8):
            score = float(scores[i-x+2].string)
            f.write('score = ' + str(score)+'\n')

            detailurl = tcc + details[j].find('a').get('href')
            r = s.get(detailurl)
            soup = BeautifulSoup(r.text.encode(r.encoding), 'html.parser')
            f.write(soup.find('pre').string + '\n')

            time.sleep(0.5)
            
            print(str(i+1) + 'Done!')
            i+=1
            if i==cnt:
                break

            
