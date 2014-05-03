## Description

In 2012 before I have written [my article](http://www.strozhevsky.com/free_docs/asn1_in_simple_words.pdf) I inspected several existing implementations of ASN.1 coder/decoder functionality. It was sad, but I found few (frankly speaking sometime not a few) mistakes in such programs delivering on a market. Of course I wrote all the mistakes to developers. But at the same time I understud that there is no freely available ASN.1 coder/decoder. Yes, there are many of such implementations but there is no implementation for all ASN.1:2008 data type together with BER encoding rules. So, I decided to create one :) Also the coder/decoder gave me a greate help on creating [my ASN.1:2008 test suite](https://github.com/YuryStrozhevsky/ASN1-2008-free-test-suite). 

## Words about BER (Basic Encoding Rules)

Almost all specialists know about ASN.1, some of them know about DER (Distinguished Encoding Rules). DER is using widely in crypto and all other stuf. But only less part of the specialist working with ASN.1 know that DER is a simple set of restrictions on a bigger set of rules - BER. That is it - BER is much bigger than DER. These are a really "basic ASN.1 rules", DER is a secondary sub-set of these rules. Please keep it in mind, take a look at my code and get all knowledge about a real (not cutted) ASN.1 power!

## ASN1js project

In 2014 I made a project [ASN1js](https://github.com/GlobalSign/ASN1.js). This is an ASN.1 BER encoder/decoder on JavaScript language. In fact all code for [ASN1js](https://github.com/GlobalSign/ASN1.js) is a "porting" from this project code, from C++ to JavScript. Also the same was using inside [PKIjs](https://github.com/GlobalSign/PKI.js) project. [ASN1js](https://github.com/GlobalSign/ASN1.js) has many improvements related to ASN.1 schema validation. Also because of JavaScript [ASN1js](https://github.com/GlobalSign/ASN1.js) has some limitations and slightly diferent code structure. For the moment I have a plans to back-port all [ASN1js](https://github.com/GlobalSign/ASN1.js) improvements to C++ project. Coming soon!

## Features of C++ ASN.1:2008 BER coder/encoder

* Really BER coder/decoder (hence DER and CER will be processed as well);
* Three formats of input/output data:
  * Binary raw data;
  * BASE-64 encoded binary raw data;
  * XER-like XML files;
* Working with all datatypes from latest ASN.1:2008 standard;
* Fully object-oriented C++ code;
* Layered code, can be easily extended;
* A bulk mode (working with many files at once);
* Special XML format (input and output) can help to understand all details of ASN. in a structured form;
* Output XML files may be used as an input for backward encoding into BER;
* All XML structures described in well-documented XSD;

## Usage

Firstly you need to create a configuration file. Schema for configuration file is inside "code/CompliConfig.xsd". Example of such configuration:
```
<?xml version="1.0" encoding="utf-8"?>
<config>
  <file_group>
    <in>
      <format>BER</format>
      <name>example.ber</name>
    </in>
    <out>
      <format>XML</format>
      <name>example.xml</name>
    </out>
  </file_group>
</config>
```

There could be many "file_group" regions (and hence many files). Possible formats:
* BER
* BER64
* XML

Usually output of the program is in XML. But please keep in mind - the output XML may be used as an input! Yes, you can parse BER (DER, CER) ASN.1 binary data, get XML file, change this XML file whatever you want and encode it back to binary. Is not it funny? :) Schema for output/input XML is in "code/CompliXML.xsd".

## Limitations

* Windows-only project;
* MSXML6 library need to be installed;

## License

Copyright (c) 2014, [Yury Strozhevsky](http://www.strozhevsky.com/)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.