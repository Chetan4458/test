
def another_function():
    for i in range(10)  # Missing colon
        print(i)

    if i > 5:  # Unused variable 'i'
        print("i is greater than 5"  # Missing closing parenthesis
    else
        print("i is less than or equal to 5")  # Missing colon

example_function()
another_function(10)  # Incorrect number of arguments

# Logical error example
result = "5" + 5  # This will raise a TypeError
