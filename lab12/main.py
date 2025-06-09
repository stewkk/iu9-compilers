#!/usr/bin/env python3

import abc
import enum
import typing
from dataclasses import dataclass
import parser_edsl as pe
import re
from pprint import pprint
import sys


class DefinitionBase(abc.ABC):
    pass


# Definition -> Struct | Enum | Union
@dataclass
class Definition:
    data: DefinitionBase


# NumType -> INT | DOUBLE | FLOAT | CHAR | SHORT | LONG
class Type(enum.Enum):
    INT = 'int'
    DOUBLE = 'double'
    FLOAT = 'float'
    CHAR = 'char'
    SHORT = 'short'
    LONG = 'long'


@dataclass
class NumVariablesDefinition:
    typename: Type
    pointer_level: int
    name_dimension: list[tuple[str, list[str|int]]]


# Struct -> STRUCT NameOpt StructFieldsOpt PointerOpt VariablesOpt ;
@dataclass
class Struct(DefinitionBase):
    name: str|None
    fields: list[Definition|NumVariablesDefinition]
    pointer_level: int
    variables: list[str]


# Expr -> NUMBER | NAME | Expr + Expr | Expr - Expr | Expr * Expr | Expr / Expr | SIZEOF ( Expr )
class Expr(abc.ABC):
    pass


@dataclass
class ExprNumber(Expr):
    number: int


@dataclass
class ExprName(Expr):
    name: str


@dataclass
class BinOpExpr(Expr):
    lhs: Expr
    op: str
    rhs: Expr


@dataclass
class UnOpExpr(Expr):
    op: str
    expr: Expr


# EnumField -> NAME EnumFieldRhsOpt
@dataclass
class EnumField:
    name: str
    rhs: Expr|None


# Enum -> ENUM NameOpt EnumFieldsOpt PointerOpt VariablesOpt ;
@dataclass
class Enum(DefinitionBase):
    name: str|None
    fields: list[EnumField]
    pointer_level: int
    variables: list[str]


# Union -> UNION NameOpt UnionFieldsOpt PointerOpt VariablesOpt ;
@dataclass
class Union(DefinitionBase):
    name: str|None
    fields: list[Definition|NumVariablesDefinition]
    pointer_level: int
    variables: list[str]


# Program -> Definition Program | Definition
@dataclass
class Program:
    definitions: list[Definition]


INTEGER = pe.Terminal('INTEGER', '[0-9]+', int, priority=7)
VARNAME = pe.Terminal('VARNAME', '[A-Za-z][A-Za-z0-9_]*', str)

def make_keyword(image):
    return pe.Terminal(image, image, lambda name: None, priority=10)

KW_INT, KW_DOUBLE, KW_FLOAT, KW_CHAR, KW_SHORT, KW_LONG = \
    map(make_keyword, 'int double float char short long'.split())

KW_SIZEOF, KW_ENUM, KW_UNION, KW_STRUCT = \
    map(make_keyword, 'sizeof enum union struct'.split())

NProgram, NDefinition, NDefinitions, NStruct, NEnum, NUnion = \
    map(pe.NonTerminal, 'Program Definition Definitions Struct Enum Union'.split())

NDefinitionOrVariable, NNumVar, NNumType, NPointerOpt, NVariables = \
    map(pe.NonTerminal, 'DefinitionOrVariable NumVar NumType PointerOpt Variables'.split())

NNameOpt, NStructFieldsOpt, NVariablesOpt, NStructFields = \
    map(pe.NonTerminal, 'NameOpt StructFieldsOpt VariablesOpt StructFields'.split())

NDefinitionsOrVariables, NVariablesTail, NEnumFieldsOpt, NSizeOf  = \
    map(pe.NonTerminal, 'DefinitionsOrVariables VariablesTail, EnumFieldsOpt SizeOf'.split())

NEnumFields, NEnumField, NEnumFieldsTail, NCommaOpt, NDimensions  = \
    map(pe.NonTerminal, 'EnumFields EnumField EnumFieldsTail CommaOpt Dimensions'.split())

NEnumFieldRhsOpt, NExpr, NUnionFieldsOpt, NUnionFields = \
    map(pe.NonTerminal, 'EnumFieldRhsOpt Expr UnionFieldsOpt UnionFields'.split())

NEnumOther, NEnumOtherOther, NEnumFieldsRest, NEnumFieldsBody = \
    map(pe.NonTerminal, 'EnumOther EnumOtherOther EnumFieldsRest EnumFieldsBody'.split())

NFactor, NTerm, NAddOp, NMulOp = \
    map(pe.NonTerminal, 'Factor Term AddOp MulOp'.split())

# Program -> Definition Program | Definition
NProgram |= NDefinitions, Program
NDefinitions |= NDefinition, NDefinitions, lambda d, p: [d]+p
NDefinitions |= lambda: []

# Definition -> Struct | Enum | Union
NDefinition |= NStruct
NDefinition |= NEnum
NDefinition |= NUnion

# DefinitionOrVariable -> Definition | NumVar
NDefinitionOrVariable |= NDefinition
NDefinitionOrVariable |= NNumVar

# NumVar -> NumType PointerOpt Variables ;
NNumVar |= NNumType, NPointerOpt, NVariables, ';', NumVariablesDefinition

# PointerOpt -> * PointerOpt | ε
NPointerOpt |= '*', NPointerOpt, lambda x: x+1
NPointerOpt |= lambda: 0

# NumType -> INT | DOUBLE | FLOAT | CHAR | SHORT | LONG
NNumType |= KW_INT, lambda: Type.INT
NNumType |= KW_DOUBLE, lambda: Type.DOUBLE
NNumType |= KW_CHAR, lambda: Type.CHAR
NNumType |= KW_SHORT, lambda: Type.SHORT
NNumType |= KW_LONG, lambda: Type.LONG

# Struct -> STRUCT NameOpt StructFieldsOpt PointerOpt VariablesOpt ;
NStruct |= KW_STRUCT, NNameOpt, NStructFieldsOpt, NPointerOpt, NVariablesOpt, ';', Struct

# NameOpt -> NAME | ε
NNameOpt |= VARNAME
NNameOpt |= lambda: ''

# VariablesOpt -> Variables | ε
NVariablesOpt |= NVariables
NVariablesOpt |= lambda: []

# StructFieldsOpt -> StructFields | ε
NStructFieldsOpt |= NStructFields
NStructFieldsOpt |= lambda: []

# StructFields -> { DefinitionsOrVariables }
NStructFields |= '{', NDefinitionsOrVariables, '}'

# DefinitionsOrVariables -> DefinitionOrVariable DefinitionsOrVariables | ε
NDefinitionsOrVariables |= NDefinitionOrVariable, NDefinitionsOrVariables, lambda d, arr: [d]+arr
NDefinitionsOrVariables |= lambda: []

# Variables -> NAME , Variables | Variables
NVariables |= VARNAME, NDimensions, ',', NVariables, lambda name, dimensions, arr: [(name, dimensions)]+arr
NVariables |= VARNAME, NDimensions, lambda name, dimensions: [(name, dimensions)]

NDimensions |= '[', NExpr, ']', NDimensions, lambda expr, other: [expr]+other
NDimensions |= lambda: []


NEnum |= KW_ENUM, VARNAME, NEnumOther, lambda name, other: Enum(name, *other)
NEnum |= KW_ENUM, NEnumOther, lambda other: Enum('', *other)

NEnumOther |= NEnumFields, NEnumOtherOther, lambda fields, other: [fields]+other
NEnumOther |= NEnumOtherOther, lambda other: [[]]+other

NEnumOtherOther |= NPointerOpt, NVariables, ';', lambda p, v: [p, v]
NEnumOtherOther |= ';', lambda: [0, []]

NEnumFields |= '{', NEnumFieldsBody

NEnumFieldsBody |= '}', lambda: []
NEnumFieldsBody |= NEnumField, NEnumFieldsRest, lambda l, r: [l]+r
NEnumFieldsRest |= ',', NEnumField, NEnumFieldsRest, lambda l, r: [l]+r
NEnumFieldsRest |= ',', '}', lambda: []
NEnumFieldsRest |= '}', lambda: []

# EnumFieldsTail -> , EnumField EnumFieldsTail | ε
NEnumFieldsTail |= ',', NEnumField, NEnumFieldsTail, lambda f, t: [f]+t
NEnumFieldsTail |= lambda: []

# EnumField -> NAME EnumFieldRhsOpt
NEnumField |= VARNAME, '=', NExpr, EnumField
NEnumField |= VARNAME, lambda name: EnumField(name, None)

# Expr -> NUMBER | NAME | Expr + Expr | Expr - Expr | Expr * Expr | Expr / Expr | SIZEOF ( VARNAME ) | - Expr | ( Expr )
NExpr |= NTerm
NExpr |= '+', NTerm, lambda t: UnOpExpr('+', t)
NExpr |= '-', NTerm, lambda t: UnOpExpr('-', t)
NExpr |= NExpr, NAddOp, NTerm, BinOpExpr
NTerm |= NFactor
NTerm |= NTerm, NMulOp, NFactor, BinOpExpr
NMulOp |= '*', lambda: '*'
NMulOp |= '/', lambda: '/'
NAddOp |= '+', lambda: '+'
NAddOp |= '-', lambda: '-'
NFactor |= INTEGER, lambda v: ExprNumber(int(v))
NFactor |= '(', NExpr, ')'
NFactor |= KW_SIZEOF, '(', NSizeOf, ')', lambda inner: UnOpExpr('sizeof', ExprName(inner))
NFactor |= VARNAME, ExprName
NSizeOf |= KW_ENUM, VARNAME, lambda y: 'enum '+y
NSizeOf |= KW_UNION, VARNAME, lambda y: 'union '+y
NSizeOf |= KW_STRUCT, VARNAME, lambda y: 'struct '+y
NSizeOf |= VARNAME
NSizeOf |= NNumType

# Union -> UNION NameOpt UnionFieldsOpt PointerOpt VariablesOpt ;
NUnion |= KW_UNION, NNameOpt, NUnionFieldsOpt, NPointerOpt, NVariablesOpt, ';', Union

# UnionFieldsOpt -> UnionFields | ε
NUnionFieldsOpt |= NUnionFields
NUnionFieldsOpt |= lambda: []

# UnionFields -> { DefinitionsOrVariables }
NUnionFields |= '{', NDefinitionsOrVariables, '}'

p = pe.Parser(NProgram)

p.add_skipped_domain('\\s')
p.add_skipped_domain(r"(?:\/\/.*)|(?:\/\*(?:.|\n)*?\*\/)")

for filename in sys.argv[1:]:
    with open(filename) as f:
        tree = p.parse_earley(f.read())
        pprint(tree)
