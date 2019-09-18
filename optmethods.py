from terminaltables import DoubleTable as Table
from pyparsing import ParseException, Word, alphas, alphanums
from fourFn import BNF, exprStack, fn, opn
import math

variables = {}

def evaluateStack(s):
    op = s.pop()
    if op == 'unary -':
        return -evaluateStack( s )
    if op in "+-*/^":
        op2 = evaluateStack( s )
        op1 = evaluateStack( s )
        return opn[op]( op1, op2 )
    elif op == "PI":
        return math.pi # 3.1415926535
    elif op == "E":
        return math.e  # 2.718281828
    elif op in fn:
        return fn[op]( evaluateStack( s ) )
    elif op[0].isalpha():
        if op in variables:
            return variables[op]
        raise Exception("invalid identifier '%s'" % op)
    else:
        return float( op )

arithExpr = BNF()
ident = Word(alphas, alphanums).setName("identifier")
assignment = ident("varname") + '=' + arithExpr
pattern = assignment | arithExpr

GOLDEN_RATIO = 0.618033988749895
HEADERS = ['K', 'a[k]', 'b[k]', 'λ[k]', 'μ[k]', \
           'F(λ[k])', 'F(μ[k])']
FIBONACHI = [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, \
             144, 233, 377, 610, 987, 1597, 2584,  \
             4181, 6765, 10946, 17711]

def __evaluate_function(arg, set=False):
    if (set):
        pattern.parseString(arg, parseAll=True)        
        return __evaluate_function
    else:
        variables['x'] = arg
        return evaluateStack(exprStack[:])

def __get_compare(mode):
    if mode == 'max':
        return lambda x, y: x < y
    elif mode == 'min':
        return lambda x, y: x > y
    else:
        raise RuntimeError('Mode is not specified.' \
                           'Use \'max\' or \'min\'.') 
         

def dihotomic_search(function, A, B, mode, eps, length):    
    step = 1   
    table = [HEADERS]
    
    f = __evaluate_function(function, set=True)
    compare = __get_compare(mode)

    while B-A > length:    
        nA = (A + B)/2 - eps
        nB = (A + B)/2 + eps
        fnA, fnB = f(nA), f(nB)        
        table.append( \
        ['%.8g' % i for i in [step, A, B, nA, nB, fnA, fnB]])
        if compare(fnA, fnB):
            A = nA
        else:
            B = nB
        step += 1
    
    # Result  
    optimal = (A+B)/2
    result = 'Оптимальное значение аргумента: {0:.8g}\n' \
             'Оптимальное значение функции: {1:.8g}\n'  \
             'Кол-во вычислений: {2}' \
             .format(optimal, f(optimal), step)
    
    return (table, result)

def golden_ratio_search(function, A, B, mode, eps, length):
    step = 1   
    table = [HEADERS]    
    
    f = __evaluate_function(function, set=True)
    compare = __get_compare(mode) 
    
    # First step
    nA = A + (1-GOLDEN_RATIO)*(B-A)
    nB = A + GOLDEN_RATIO*(B-A)
    fnA, fnB = f(nA), f(nB)
    table.append( \
    ['%.8g' % i for i in [step, A, B, nA, nB, fnA, fnB]])
    step += 1
    
    # Remaining steps
    while B-A > length:                    
        if compare(fnA, fnB):
            A, nA = nA, nB
            nB = A + GOLDEN_RATIO*(B-A)
        else:
            B, nB = nB, nA
            nA =  A + (1-GOLDEN_RATIO)*(B-A)
            
        fnA, fnB = f(nA), f(nB)  
        table.append( \
        ['%.8g' % i for i in [step, A, B, nA, nB, fnA, fnB]])
        step += 1
    
    # Result
    optimal = (nA+nB)/2
    result = 'Оптимальное значение аргумента: {0:.8g}\n' \
             'Оптимальное значение функции: {1:.8g}\n' \
             'Кол-во вычислений: {2}' \
             .format(optimal, f(optimal), step)
    
    return (table, result)

def fibonachi_search(function, A, B, mode, eps, length):
    step = 1    
    n = 0
    table = [HEADERS]
    
    f = __evaluate_function(function, set=True)
    compare = __get_compare(mode) 
    
    while FIBONACHI[n] < (B-A)/length: n += 1
    
    # First step
    nA = A + FIBONACHI[n-2]*(B-A)/FIBONACHI[n]
    nB = A + FIBONACHI[n-1]*(B-A)/FIBONACHI[n]    
    fnA, fnB = f(nA), f(nB)        
    table.append(['%.8g' % i for i in \
                 [step, A, B, nA, nB, fnA, fnB]])
    
    # Remaining steps
    while step < n-1:                
        step += 1
        if compare(fnA, fnB):
            A, nA = nA, nB
            nB = A + FIBONACHI[n-step]*(B-A) \
                     /FIBONACHI[n-step+1]
        else:
            B, nB = nB, nA
            nA = A + FIBONACHI[n-step-1]*(B-A) \
                     /FIBONACHI[n-step+1]
        fnA, fnB = f(nA), f(nB)        
        table.append(['%.8g' % i for i in \
                     [step, A, B, nA, nB, fnA, fnB]])    
                
    # Last step
    step += 1
    nB = nA + eps
    fnA, fnB = f(nA), f(nB)
    if compare(fnA, fnB):
        A = nA
    else:
        B = nB
    table.append( \
    ['%.8g' % i for i in [step, A, B, nA, nB, fnA, fnB]]) 
    
    # Result
    optimal = (A+B)/2
    result = 'Оптимальное значение аргумента: {0:.8g}\n' \
             'Оптимальное значение функции: {1:.8g}\n' \
             'Кол-во вычислений: {2}' \
             .format(optimal, f(optimal), step)
    
    return (table, result)

def get_printable_table(table):
    return Table(table[0]).table + '\n' + table[1]
    
