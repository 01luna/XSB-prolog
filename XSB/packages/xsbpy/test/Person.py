class Person:
  def __init__(self, name, age):
    self.name = name
    self.age = age

  def func0(self):
    return("Hello my name is " + self.name)

  def func1(self,myclass):
    return("Hello my name is " + self.name + " and I'm a " + myclass)

#p1 = Person("John", 36)
#p1.myfunc() 
