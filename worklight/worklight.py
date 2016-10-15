from icalendar import Calendar
import ConfigParser
import urllib2
from datetime import datetime,timedelta
import pytz
from dateutil.rrule import rrulestr
import operator
import time
import serial

def to_date(date):
    try:
        return date.date()
    except:
        return date

def getCalendarData(calendar_url):
  req = urllib2.Request(calendar_url)
  response = urllib2.urlopen(req)
  data = response.read()

  today = datetime.now().date()
  in_future = today + timedelta(days=15)
  cal = Calendar.from_ical(data)

  events=[]
  for event in cal.walk('vevent'):
    if not "QUIET TIME" in upper(event.get("summary")): continue
    if (event.get('rrule')):
      start = event.get('dtstart').dt
      duration = event.get('dtend').dt - start
      try:
          rrule = rrulestr(event.get('rrule').to_ical(), dtstart=start)
          start = rrule.after(datetime.now(pytz.UTC))
      except:
          rrule = rrulestr(event.get('rrule').to_ical(), dtstart=start)
          start = rrule.after(datetime.now().replace(tzinfo=None))
          if start: start.replace(tzinfo=pytz.UTC)
      if start:
          end = start + duration
      else:
          continue
    else:
      start = event.get('dtstart').dt
      end = event.get('dtend').dt

    if ((to_date(start) >= today) and (to_date(end) <= in_future)):
      events.append({
        "start": start,
        "end": end,
        "summary": event.get("summary"),
      })

  return events

config = ConfigParser.RawConfigParser()
config.read("worklight.cfg")
calendar_url = config.get("default", "url")
light_device = config.get("default", "device")

print str(calendar_url)
print str(light_device)

events = getCalendarData(calendar_url)
events[0]["start"] = datetime.now(pytz.UTC)
print "upcoming events I will switch the light on while:"
for event in sorted(events, key=operator.itemgetter("start")):
    print str(event["start"]) + " - " + str(event["end"]) + ": " + event["summary"]

while (events):
    if events[0]["start"] < datetime.now(pytz.UTC):
        event = events[0]
        events = events[1:]
        print "event just started: " + str(event)

        message = 'Q ' + str(int((event["end"] - event["start"]).total_seconds() / 60)) + ' RED\n'
        print str(message)
        ser = serial.Serial(light_device, 9600)
        time.sleep(3)
        ser.write(message)
        ser.close()
    print "sleeping..."
    time.sleep(10)
