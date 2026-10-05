#Test example 
test_cases = [
    ([7], 1),
    ([3,4,7], 1),
    ([3,4,1,2,4], 2)
]

def run_test_suite(callback: function, test_cases: list[tuple], t: int|str = None) -> int|str:
    for i in range(len(test_cases)):
        if t:
            result = callback(test_cases[i][0], t)    
        else:
            result = callback(test_cases[i][0])
        if result == test_cases[i][1]:
            print(f"PASSED for {test_cases[i][1]} == {result}")
        else:
            print(f"FAILED for {test_cases[i][1]} == {result}")
