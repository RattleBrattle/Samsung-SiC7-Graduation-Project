import os

def say(text):
    os.system(f'flite -t "{text}"')
