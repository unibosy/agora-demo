import os
import re
import time
import json
import collections
import copy
from os import listdir

global_settings = ''
robot_settings = []
robots = []
interval = 0
configFile = r'./config.json'

def runCmd(s):
    # print s
    os.system(s + ' 2>&1')

def loadCfg():
    jsonfile = file(configFile)
    jsonobj = json.load(jsonfile)
    global global_settings
    global robot_settings
    global robots
    global interval

    global_settings = jsonobj["global_settings"]
    interval = jsonobj["global_settings"]["start_interval_sec"]

    for i in range(len(jsonobj["robot_settings"])):
        robot_settings.append(jsonobj["robot_settings"][i])

    for i in range(len(jsonobj["robots"])):
        robots.append(jsonobj["robots"][i])

    jsonfile.close

def runRobots():
    strPatternQuot = re.compile('\"')
    strPatternSpace = re.compile(' ')
    global_settings_text = json.dumps(global_settings, separators=(',',':'))
    global_settings_text = strPatternQuot.sub('\\"',global_settings_text)
    global_settings_text = strPatternSpace.sub('',global_settings_text)

    for i in range(len(robots)):
        robot_settings_text = json.dumps(robot_settings[robots[i]], separators=(',',':'))
        robot_settings_text = strPatternQuot.sub('\\"',robot_settings_text)
        robot_settings_text = strPatternSpace.sub('',robot_settings_text)

        print("\n############# ROBOT %d #############" %i)
        cmd = './AgoraPremiumRobot --global_settings %s --robot_settings %s &' % (global_settings_text, robot_settings_text)

        runCmd(cmd)
        time.sleep(interval)

loadCfg()
runRobots()