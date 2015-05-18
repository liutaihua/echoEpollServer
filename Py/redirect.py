import sys
import echo

class MyStdOut(object):
    def write(self, s):
        if type(s) == unicode:
            s = s.encode('utf-8')
        echo.PyStdOut(s)

def redirect_output():
    sys.stdout = MyStdOut()
    sys.stderr = MyStdOut()
    print "init python ok!"
