



def two_sum(array, target):
    for i in range(len(array)):
        for j in range(len(array)):
            if array[j]+array[i] == target:
                return (i,j)
    return None

# two sum 
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

# palindrome 
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

#greedy running state// tracking dynamic state in a single pass
#O(n)
print(max_profit([7, 1, 5, 3, 6, 4]))