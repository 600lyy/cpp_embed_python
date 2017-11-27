#!/usr/bin/env python3
from datetime import datetime
import logging
import gc
from logging.handlers import RotatingFileHandler
from transitions.extensions import LockedHierarchicalMachine as Machine
try:
    import cppwrapper as cppwrapper 
except ImportError as e:
    logger.error(e)

LOG_FILENAME = 'run.log'

def get_logger():
    logger = logging.getLogger('root')
    FORMAT = "%(asctime)s %(levelname)s [%(filename)s:%(lineno)s - " \
             "%(funcName)20s() ] %(message)s"
    logging.basicConfig(format=FORMAT)
    handler = RotatingFileHandler(
        LOG_FILENAME, maxBytes=1e8, backupCount=10, encoding='utf-8'
    )
    handler.setFormatter(logging.Formatter(FORMAT))
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)
    return logger

logger = get_logger()

first_run = True 

def send_iox_signal(eventid, value):
    statemachine_interface.funcSendIOXSignal(eventid, value)

def wortask_done():
    statemachine_interface.funcWorktaskDone()

def subscribe_iox_signal(eventid, event):
    statemachine_interface.funcSubscribeIOXSignal(eventid, event)

def send_event(event):
    global machine
    global first_run
    if machine and isinstance(machine, Machine): 
        logger.info('Sending Event: ' + event)
        machine.trigger(event)
        if first_run:
            machine.advance()
            first_run = False
    else:
        logger.error('No instance of Machine exists')
        raise RuntimeError('machine does not exist')

def get_state():
    global machine
    return machine.state

