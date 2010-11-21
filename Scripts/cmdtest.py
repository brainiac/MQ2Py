import mq2

def FunctionTest(str):
	print str
	
mq2.AddCommand("/pytest", FunctionTest);

mq2.DoCommand("/pytest whats up")

mq2.RemoveCommand("/pytest")