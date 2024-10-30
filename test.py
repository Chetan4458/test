url='https://www.google.com'
def example_function():
    print("This is a sample function")

def another_function():
    for i in range(10) : # Added colon
        print(i)

    if i > 5:  # Unused variable 'i'
        print("i is greater than 5" ) # Added closing parenthesis
    else:
        print("i is less than or equal to 5")  

example_function()
#another_function(10)  

# No logical error 
result = 5  
