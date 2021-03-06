#!/usr/bin/python3
from string import Template
import itertools
import sys
dtypes = ["Int", "UInt", "Double", "Float", "Char", "UChar"]
dtypeMap = dict(Int="int32_t",UInt="uint32_t",Double="double",
                Float="float",Char="int8_t",UChar="uint8_t")

def colMagic(dtypes, n):
    combs = list(itertools.combinations(dtypes,n))
    out = ""
    for c in range(0,len(combs)):
         out += "    if (contains(dtypes,"
         for d in range(0,len(combs[c])-1):
             out += Template("DataType::$dt) && contains(dtypes,").substitute(dt=combs[c][d])
         out += Template("DataType::$dt)){colMagic_ = $cm; return;}\n").substitute(dt=combs[c][-1],cm=c)
    return(out)

def fullMagic(dtypes):
    out = "switch(nDtypes_){\n"
    for i in range(1,len(dtypes)):
        out += Template("  case $im:\n  {\n").substitute(im=i)
        out += colMagic(dtypes,i)
        out += "  }\n"
    out += "  case 6: {colMagic_ = 0; return;}\n}\nthrow std::runtime_error(\"too many datatypes\");"
    return(out)

def templatizeBlock(dtypes,i,codeblock1,codeblock2,owner):
    combs = list(itertools.combinations(dtypes,i))
    out = Template("    switch($own->colMagic_){\n").substitute(own=owner)
    for c in range(0,len(combs)):
        out += Template("    case $im:{\n").substitute(im=c)
        typesl = map(lambda x: dtypeMap[x],combs[c])
        types = ",".join(typesl)
        out += Template("    $tps<").substitute(tps=codeblock1)
        out += types
        out += Template(">$tps\n    }\n").substitute(tps=codeblock2)
    out += "    }\n"
    return(out)

def templatize(dtypes,codeblock1,codeblock2,owner="this"):
    out = Template("switch($own->nDtypes_){\n").substitute(own=owner)
    for i in range(1,len(dtypes)+1):
        out += Template("  case $im:\n  {\n").substitute(im=i)
        out += templatizeBlock(dtypes,i,codeblock1,codeblock2,owner)
        out += "  }\n"
    out += "}\n"
    return(out)

def template1(content):
    tpLoc = content.find("$TPC1")
    while tpLoc != -1:
        commaLoc = content.find("\,",tpLoc)
        block1 = content[tpLoc+11:commaLoc]
        endLoc = content.find("\\right)",commaLoc)
        block2 = content[commaLoc+2:endLoc]
        # print("block1: {}, block2: {}".format(block1, block2))
        # sys.exit(1)
        expand = templatize(dtypes,block1,block2)
        content = content[0:tpLoc]+expand+content[endLoc+8:]
        tpLoc = content.find("$TPC1")
    return(content)

def template2(content):
    tpLoc = content.find("$TPC2")
    while tpLoc != -1:
        commaLoc = content.find("\,",tpLoc)
        block1 = content[tpLoc+11:commaLoc]
        endLoc = content.find("\\right)",commaLoc)
        block2 = content[commaLoc+2:endLoc]
        # print("block1: {}, block2: {}".format(block1, block2))
        # sys.exit(1)
        expand = templatize(dtypes,block1,block2,"parent_")
        content = content[0:tpLoc]+expand+content[endLoc+8:]
        tpLoc = content.find("$TPC2")
    return(content)


f = open("DF.input","r")
content = f.read()
content = template1(content)
content = template2(content)
out = Template(content).substitute(COL_TP=fullMagic(dtypes))
f2 = open("DF.tpp","w")
f2.write(out)
