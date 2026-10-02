



def two_sum(array, target):
    for i in range(len(array)):
        for j in range(len(array)):
            if array[j]+array[i] == target:
                return (i,j)
    return None

# two sum // hash map
# O(n^2)
# print(two_sum([2,11,15],9))

def two_sum_faster(array, target):
    seen = dict()
    for index, value in enumerate(array):
        complement = target - value
        if complement in seen:
            return (index, seen[complement])
        seen[value] = index
    return None

#print(two_sum_faster([2,7,11,15],9))
# two sum 
# O(n)





def palindrome(text):
    left = 0
    right = len(text) - 1
    while left < right:
        while left < right and not text[left].isalpha():
            left += 1
        while left < right and not text[right].isalpha():
            right -= 1
        if text[right].lower() != text[left].lower():
            return False
        left += 1
        right -= 1
    return True

# palindrome // two pointers 
# O(n)
#print(palindrome("A man, a plan, a canal: Panama"))




def max_profit(sequence):
    max_profit = 0
    min_price = float('inf')
    for i in range(len(sequence)):
        if sequence[i] < min_price:
            min_price = sequence[i]
        else:
            if sequence[i] - min_price > max_profit:
                max_profit = sequence[i] - min_price
    return max_profit

#greedy running state// tracking dynamic state in a single pass // sliding window
#O(n)
print(max_profit([7, 1, 5, 3, 6, 4]))



def seen_twice(array):
    seen = dict()
    for i,v in enumerate(array):
        if v in seen:
            return True 
        seen[v] = i
    return False
#O(n) hash map, well could be done with set as well or any other way, this are easy on purpose 


def reverse_string(array: str):
    left = 0
    right = len(array) - 1
    temp = ""
    while left < right:
        temp = array[left]
        array[left] = array[right] 
        array[right] = temp
        left += 1
        right -= 1
        temp = ""
    return array 
#O(n) two pointers 

def max_consecutive(array: int):
    max_cons = 0
    curr_cons = 0
    for _, value in enumerate(array):
        if value == 1:
            curr_cons += 1
            if curr_cons > max_cons:
                max_cons = curr_cons
        else:
            curr_cons = 0
    return max_cons
#O(n) greedy running state, dynamic state



def search_target(a: int, t:int):
    left = 0
    right = len(a) - 1
    while left <= right:
        mid = left + (right - left) // 2
        if t == a[mid]:
            return mid
        elif t > a[mid]:
            left = mid + 1
        else:
            right = mid - 1
    return -1

#O(log n) binary search, with L + (R - L)/2