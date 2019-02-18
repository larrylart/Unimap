// Reminder: Modify typemap.dat to customize the header file generated by wsdl2h
/* unimap_service.h
   Generated by wsdl2h 1.2.16 from unimap_service.wsdl and typemap.dat
   2013-04-18 16:54:16 GMT

   DO NOT INCLUDE THIS FILE DIRECTLY INTO YOUR PROJECT BUILDS
   USE THE soapcpp2-GENERATED SOURCE CODE FILES FOR YOUR PROJECT BUILDS

   gSOAP XML Web services tools.
   Copyright (C) 2001-2010 Robert van Engelen, Genivia Inc. All Rights Reserved.
   Part of this software is released under one of the following licenses:
   GPL or Genivia's license for commercial use.
*/

/** @page page_notes Build Notes

NOTE:

 - Run soapcpp2 on unimap_service.h to generate the SOAP/XML processing logic.
   Use soapcpp2 option -I to specify paths for #import
   To build with STL, 'stlvector.h' is imported from 'import' dir in package.
   Use soapcpp2 option -i to generate improved proxy and server classes.
 - Use wsdl2h options -c and -s to generate pure C code or C++ code without STL.
 - Use 'typemap.dat' to control namespace bindings and type mappings.
   It is strongly recommended to customize the names of the namespace prefixes
   generated by wsdl2h. To do so, modify the prefix bindings in the Namespaces
   section below and add the modified lines to 'typemap.dat' to rerun wsdl2h.
 - Use Doxygen (www.doxygen.org) on this file to generate documentation.
 - Use wsdl2h options -nname and -Nname to globally rename the prefix 'ns'.
 - Use wsdl2h option -d to enable DOM support for xsd:anyType.
 - Use wsdl2h option -g to auto-generate readers and writers for root elements.
 - Struct/class members serialized as XML attributes are annotated with a '@'.
 - Struct/class members that have a special role are annotated with a '$'.

WARNING:

   DO NOT INCLUDE THIS FILE DIRECTLY INTO YOUR PROJECT BUILDS.
   USE THE SOURCE CODE FILES GENERATED BY soapcpp2 FOR YOUR PROJECT BUILDS:
   THE soapStub.h FILE CONTAINS THIS CONTENT WITHOUT ANNOTATIONS.

LICENSE:

@verbatim
--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2000-2010, Robert van Engelen, Genivia Inc. All Rights Reserved.

This software is released under one of the following two licenses:
1) GPL or 2) Genivia's license for commercial use.
--------------------------------------------------------------------------------
1) GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
2) A commercial-use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
@endverbatim

*/


//gsoapopt w

/******************************************************************************\
 *                                                                            *
 * Definitions                                                                *
 *   urn:UnimapSoapXml                                                        *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Import                                                                     *
 *                                                                            *
\******************************************************************************/


// STL vector containers (use option -s to remove STL dependency)
#import "stlvector.h"

/******************************************************************************\
 *                                                                            *
 * Schema Namespaces                                                          *
 *                                                                            *
\******************************************************************************/


/* NOTE:

It is strongly recommended to customize the names of the namespace prefixes
generated by wsdl2h. To do so, modify the prefix bindings below and add the
modified lines to typemap.dat to rerun wsdl2h:

ns = "urn:UnimapSoapXml"

*/

#define SOAP_NAMESPACE_OF_ns	"urn:UnimapSoapXml"
//gsoap ns    schema namespace:	urn:UnimapSoapXml
//gsoap ns    schema form:	unqualified

/******************************************************************************\
 *                                                                            *
 * Built-in Schema Types and Top-Level Elements and Attributes                *
 *                                                                            *
\******************************************************************************/


/// Primitive built-in type "xs:integer"
typedef std::string xsd__integer;

/******************************************************************************\
 *                                                                            *
 * Forward Declarations                                                       *
 *                                                                            *
\******************************************************************************/



//  Forward declaration of class ns__ResponseStatusType.
class ns__ResponseStatusType;

//  Forward declaration of class ns__ImgDataHeaderType.
class ns__ImgDataHeaderType;

//  Forward declaration of class ns__ImgObjDetectedType.
class ns__ImgObjDetectedType;

//  Forward declaration of class ns__ImageSolveRequestType.
class ns__ImageSolveRequestType;

//  Forward declaration of class ns__ImageSolveResponseType.
class ns__ImageSolveResponseType;

//  Forward declaration of class ns__ScopePositionSetRequestType.
class ns__ScopePositionSetRequestType;

//  Forward declaration of class ns__ScopePositionSetResponseType.
class ns__ScopePositionSetResponseType;

//  Forward declaration of class ns__ScopePositionGetRequestType.
class ns__ScopePositionGetRequestType;

//  Forward declaration of class ns__ScopePositionGetResponseType.
class ns__ScopePositionGetResponseType;

//  Forward declaration of class ns__CameraImageTakeRequestType.
class ns__CameraImageTakeRequestType;

//  Forward declaration of class ns__CameraImageTakeResponseType.
class ns__CameraImageTakeResponseType;

//  Forward declaration of class ns__ScopeFocusAdjustRequestType.
class ns__ScopeFocusAdjustRequestType;

//  Forward declaration of class ns__ScopeFocusAdjustResponseType.
class ns__ScopeFocusAdjustResponseType;

/******************************************************************************\
 *                                                                            *
 * Schema Types and Top-Level Elements and Attributes                         *
 *   urn:UnimapSoapXml                                                        *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Complex Types and Top-Level Elements                                *
 *   urn:UnimapSoapXml                                                        *
 *                                                                            *
\******************************************************************************/


/// "urn:UnimapSoapXml":ResponseStatusType is a complexType.
class ns__ResponseStatusType
{ public:
/// Element ResponseResult of type xs:string.
    std::string                          ResponseResult                 1;	///< Required element.
/// Element ResponseCode of type xs:short.
    short                                ResponseCode                   1;	///< Required element.
/// Vector of std::string with length 0..unbounded
    std::vector<std::string            > ResponseMessage                0;
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ImgDataHeaderType is a complexType.
class ns__ImgDataHeaderType
{ public:
/// Element ObjDetected of type xs:integer.
    xsd__integer                         ObjDetected                    1;	///< Required element.
/// Element ObjMatched of type xs:integer.
    xsd__integer                         ObjMatched                     1;	///< Required element.
/// Element TargetName of type xs:string.
    std::string                          TargetName                     1;	///< Required element.
/// Element OrigRa of type xs:double.
    double                               OrigRa                         1;	///< Required element.
/// Element OrigDec of type xs:double.
    double                               OrigDec                        1;	///< Required element.
/// Element FieldWidth of type xs:double.
    double                               FieldWidth                     1;	///< Required element.
/// Element FieldHeight of type xs:double.
    double                               FieldHeight                    1;	///< Required element.
/// Element MinMag of type xs:double.
    double                               MinMag                         1;	///< Required element.
/// Element MaxMag of type xs:double.
    double                               MaxMag                         1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ImgObjDetectedType is a complexType.
class ns__ImgObjDetectedType
{ public:
/// Element id of type xs:integer.
    xsd__integer                         id                             1;	///< Required element.
/// Element name of type xs:string.
    std::string                          name                           1;	///< Required element.
/// Element x of type xs:double.
    double                               x                              1;	///< Required element.
/// Element y of type xs:double.
    double                               y                              1;	///< Required element.
/// Element ra of type xs:double.
    double                               ra                             1;	///< Required element.
/// Element dec of type xs:double.
    double                               dec                            1;	///< Required element.
/// Element mag of type xs:double.
    double                               mag                            1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ImageSolveRequestType is a complexType.
class ns__ImageSolveRequestType
{ public:
/// Element ImageName of type xs:string.
    std::string                          ImageName                      1;	///< Required element.
/// Element ImagePath of type xs:string.
    std::string                          ImagePath                      1;	///< Required element.
/// Element SearchCatalog of type xs:string.
    std::string                          SearchCatalog                  1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ImageSolveResponseType is a complexType.
class ns__ImageSolveResponseType
{ public:
/// Element ResponseStatus of type "urn:UnimapSoapXml":ResponseStatusType.
    ns__ResponseStatusType*              ResponseStatus                 1;	///< Nullable pointer.
/// Element ImgDataHeader of type "urn:UnimapSoapXml":ImgDataHeaderType.
    ns__ImgDataHeaderType*               ImgDataHeader                  1;	///< Nullable pointer.
/// Vector of ns__ImgObjDetectedType* with length 0..unbounded
    std::vector<ns__ImgObjDetectedType*> vectImgObjDetected             0;	///< Nullable pointer.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopePositionSetRequestType is a complexType.
class ns__ScopePositionSetRequestType
{ public:
/// Element ScopeId of type xs:integer.
    xsd__integer                         ScopeId                        1;	///< Required element.
/// Element RA of type xs:double.
    double                               RA                             1;	///< Required element.
/// Element DEC of type xs:double.
    double                               DEC                            1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopePositionSetResponseType is a complexType.
class ns__ScopePositionSetResponseType
{ public:
/// Element ResponseStatus of type "urn:UnimapSoapXml":ResponseStatusType.
    ns__ResponseStatusType*              ResponseStatus                 1;	///< Nullable pointer.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopePositionGetRequestType is a complexType.
class ns__ScopePositionGetRequestType
{ public:
/// Element ScopeId of type xs:integer.
    xsd__integer                         ScopeId                        1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopePositionGetResponseType is a complexType.
class ns__ScopePositionGetResponseType
{ public:
/// Element ResponseStatus of type "urn:UnimapSoapXml":ResponseStatusType.
    ns__ResponseStatusType*              ResponseStatus                 1;	///< Nullable pointer.
/// Element RA of type xs:double.
    double                               RA                             1;	///< Required element.
/// Element DEC of type xs:double.
    double                               DEC                            1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":CameraImageTakeRequestType is a complexType.
class ns__CameraImageTakeRequestType
{ public:
/// Element ExposureTime of type xs:integer.
    xsd__integer                         ExposureTime                   1;	///< Required element.
/// Element NoOfExposures of type xs:integer.
    xsd__integer                         NoOfExposures                  1;	///< Required element.
/// Element ExposuresDelay of type xs:integer.
    xsd__integer                         ExposuresDelay                 1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":CameraImageTakeResponseType is a complexType.
class ns__CameraImageTakeResponseType
{ public:
/// Element ResponseStatus of type "urn:UnimapSoapXml":ResponseStatusType.
    ns__ResponseStatusType*              ResponseStatus                 1;	///< Nullable pointer.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopeFocusAdjustRequestType is a complexType.
class ns__ScopeFocusAdjustRequestType
{ public:
/// Element Position of type xs:string.
    std::string                          Position                       1;	///< Required element.
/// Element Speed of type xs:string.
    std::string                          Speed                          1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/// "urn:UnimapSoapXml":ScopeFocusAdjustResponseType is a complexType.
class ns__ScopeFocusAdjustResponseType
{ public:
/// Element ResponseStatus of type "urn:UnimapSoapXml":ResponseStatusType.
    ns__ResponseStatusType*              ResponseStatus                 1;	///< Nullable pointer.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Elements                                              *
 *   urn:UnimapSoapXml                                                        *
 *                                                                            *
\******************************************************************************/


/// Top-level root element "urn:UnimapSoapXml":ImageSolveRequest of type "urn:UnimapSoapXml":ImageSolveRequestType.
/// Note: use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.

/// Top-level root element "urn:UnimapSoapXml":ImageSolveResponse of type "urn:UnimapSoapXml":ImageSolveResponseType.
/// Note: use wsdl2h option -g to auto-generate a top-level root element declaration and processing code.

/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Attributes                                            *
 *   urn:UnimapSoapXml                                                        *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Services                                                                   *
 *                                                                            *
\******************************************************************************/


//gsoap ns   service name:	UnimapSoapXmlBinding 
//gsoap ns   service type:	UnimapSoapXmlPort 
//gsoap ns   service port:	http://127.0.0.1:11011/unimap_service.cgi 
//gsoap ns   service namespace:	urn:UnimapSoapXml 
//gsoap ns   service transport:	http://schemas.xmlsoap.org/soap/http 

/** @mainpage Unimap Definitions

@section Unimap_bindings Service Bindings
  - @ref UnimapSoapXmlBinding

@section Unimap_more More Information
- @ref page_notes "Notes"
- @ref page_XMLDataBinding "XML Data Binding"
- @ref SOAP_ENV__Header "SOAP Header Content"
- @ref SOAP_ENV__Detail "SOAP Fault Detail Content" (when applicable)


*/

/**

@page UnimapSoapXmlBinding Binding "UnimapSoapXmlBinding"

@section UnimapService_service Service Documentation "UnimapService"
Unimap Soap Xml Service

@section UnimapSoapXmlBinding_operations Operations of Binding  "UnimapSoapXmlBinding"
  - @ref __ns__ImageSolve
  - @ref __ns__ScopePositionSet
  - @ref __ns__ScopePositionGet
  - @ref __ns__CameraImageTake
  - @ref __ns__ScopeFocusAdjust

@section UnimapSoapXmlBinding_ports Endpoints of Binding  "UnimapSoapXmlBinding"
  - http://127.0.0.1:11011/unimap_service.cgi

Note: use wsdl2h option -N to change the service binding prefix name

*/

/******************************************************************************\
 *                                                                            *
 * Service Binding                                                            *
 *   UnimapSoapXmlBinding                                                     *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns__ImageSolve                                                         *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns__ImageSolve" of service binding "UnimapSoapXmlBinding"

/**

Operation details:

Service definition of function ns__ImageSolve
  - SOAP document/literal style
  - SOAP action="urn:UnimapSoapXml/ImageSolve"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns__ImageSolve(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    ns__ImageSolveRequestType*          ns__ImageSolveRequest,
    // response parameters:
    ns__ImageSolveResponseType*         ns__ImageSolveResponse
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns__ImageSolve(
    struct soap *soap,
    // request parameters:
    ns__ImageSolveRequestType*          ns__ImageSolveRequest,
    // response parameters:
    ns__ImageSolveResponseType*         ns__ImageSolveResponse
  );
@endcode

C++ proxy class (defined in soapUnimapSoapXmlBindingProxy.h):
@code
  class UnimapSoapXmlBindingProxy;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use proxy classes;

C++ service class (defined in soapUnimapSoapXmlBindingService.h):
@code
  class UnimapSoapXmlBindingService;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use service classes;

*/

//gsoap ns   service method-style:	ImageSolve document
//gsoap ns   service method-encoding:	ImageSolve literal
//gsoap ns   service method-action:	ImageSolve urn:UnimapSoapXml/ImageSolve
int __ns__ImageSolve(
    ns__ImageSolveRequestType*          ns__ImageSolveRequest,	///< Request parameter
    ns__ImageSolveResponseType*         ns__ImageSolveResponse	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns__ScopePositionSet                                                   *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns__ScopePositionSet" of service binding "UnimapSoapXmlBinding"

/**

Operation details:

Service definition of function ns__ScopePositionSetRequest
  - SOAP document/literal style
  - SOAP action="urn:UnimapSoapXml#ScopePositionSetRequest"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns__ScopePositionSet(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    ns__ScopePositionSetRequestType*    input,
    // response parameters:
    ns__ScopePositionSetResponseType*   result
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns__ScopePositionSet(
    struct soap *soap,
    // request parameters:
    ns__ScopePositionSetRequestType*    input,
    // response parameters:
    ns__ScopePositionSetResponseType*   result
  );
@endcode

C++ proxy class (defined in soapUnimapSoapXmlBindingProxy.h):
@code
  class UnimapSoapXmlBindingProxy;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use proxy classes;

C++ service class (defined in soapUnimapSoapXmlBindingService.h):
@code
  class UnimapSoapXmlBindingService;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use service classes;

*/

//gsoap ns   service method-style:	ScopePositionSet document
//gsoap ns   service method-encoding:	ScopePositionSet literal
//gsoap ns   service method-action:	ScopePositionSet urn:UnimapSoapXml#ScopePositionSetRequest
int __ns__ScopePositionSet(
    ns__ScopePositionSetRequestType*    input,	///< Request parameter
    ns__ScopePositionSetResponseType*   result	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns__ScopePositionGet                                                   *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns__ScopePositionGet" of service binding "UnimapSoapXmlBinding"

/**

Operation details:

Service definition of function ns__ScopePositionGetRequest
  - SOAP document/literal style
  - SOAP action="urn:UnimapSoapXml#ScopePositionGetRequest"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns__ScopePositionGet(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    ns__ScopePositionGetRequestType*    input,
    // response parameters:
    ns__ScopePositionGetResponseType*   result
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns__ScopePositionGet(
    struct soap *soap,
    // request parameters:
    ns__ScopePositionGetRequestType*    input,
    // response parameters:
    ns__ScopePositionGetResponseType*   result
  );
@endcode

C++ proxy class (defined in soapUnimapSoapXmlBindingProxy.h):
@code
  class UnimapSoapXmlBindingProxy;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use proxy classes;

C++ service class (defined in soapUnimapSoapXmlBindingService.h):
@code
  class UnimapSoapXmlBindingService;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use service classes;

*/

//gsoap ns   service method-style:	ScopePositionGet document
//gsoap ns   service method-encoding:	ScopePositionGet literal
//gsoap ns   service method-action:	ScopePositionGet urn:UnimapSoapXml#ScopePositionGetRequest
int __ns__ScopePositionGet(
    ns__ScopePositionGetRequestType*    input,	///< Request parameter
    ns__ScopePositionGetResponseType*   result	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns__CameraImageTake                                                    *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns__CameraImageTake" of service binding "UnimapSoapXmlBinding"

/**

Operation details:

Service definition of function ns__CameraImageTakeRequest
  - SOAP document/literal style
  - SOAP action="urn:UnimapSoapXml#CameraImageTakeRequest"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns__CameraImageTake(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    ns__CameraImageTakeRequestType*     input,
    // response parameters:
    ns__CameraImageTakeResponseType*    result
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns__CameraImageTake(
    struct soap *soap,
    // request parameters:
    ns__CameraImageTakeRequestType*     input,
    // response parameters:
    ns__CameraImageTakeResponseType*    result
  );
@endcode

C++ proxy class (defined in soapUnimapSoapXmlBindingProxy.h):
@code
  class UnimapSoapXmlBindingProxy;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use proxy classes;

C++ service class (defined in soapUnimapSoapXmlBindingService.h):
@code
  class UnimapSoapXmlBindingService;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use service classes;

*/

//gsoap ns   service method-style:	CameraImageTake document
//gsoap ns   service method-encoding:	CameraImageTake literal
//gsoap ns   service method-action:	CameraImageTake urn:UnimapSoapXml#CameraImageTakeRequest
int __ns__CameraImageTake(
    ns__CameraImageTakeRequestType*     input,	///< Request parameter
    ns__CameraImageTakeResponseType*    result	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * Service Operation                                                          *
 *   __ns__ScopeFocusAdjust                                                   *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns__ScopeFocusAdjust" of service binding "UnimapSoapXmlBinding"

/**

Operation details:

Service definition of function ns__ScopeFocusAdjustRequest
  - SOAP document/literal style
  - SOAP action="urn:UnimapSoapXml#ScopeFocusAdjustRequest"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns__ScopeFocusAdjust(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    ns__ScopeFocusAdjustRequestType*    input,
    // response parameters:
    ns__ScopeFocusAdjustResponseType*   result
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns__ScopeFocusAdjust(
    struct soap *soap,
    // request parameters:
    ns__ScopeFocusAdjustRequestType*    input,
    // response parameters:
    ns__ScopeFocusAdjustResponseType*   result
  );
@endcode

C++ proxy class (defined in soapUnimapSoapXmlBindingProxy.h):
@code
  class UnimapSoapXmlBindingProxy;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use proxy classes;

C++ service class (defined in soapUnimapSoapXmlBindingService.h):
@code
  class UnimapSoapXmlBindingService;
@endcode
Important: use soapcpp2 option '-i' to generate greatly improved and easy-to-use service classes;

*/

//gsoap ns   service method-style:	ScopeFocusAdjust document
//gsoap ns   service method-encoding:	ScopeFocusAdjust literal
//gsoap ns   service method-action:	ScopeFocusAdjust urn:UnimapSoapXml#ScopeFocusAdjustRequest
int __ns__ScopeFocusAdjust(
    ns__ScopeFocusAdjustRequestType*    input,	///< Request parameter
    ns__ScopeFocusAdjustResponseType*   result	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * XML Data Binding                                                           *
 *                                                                            *
\******************************************************************************/


/**

@page page_XMLDataBinding XML Data Binding

SOAP/XML services use data bindings contractually bound by WSDL and auto-
generated by wsdl2h and soapcpp2 (see Service Bindings). Plain data bindings
are adopted from XML schemas as part of the WSDL types section or when running
wsdl2h on a set of schemas to produce non-SOAP-based XML data bindings.

The following readers and writers are C/C++ data type (de)serializers auto-
generated by wsdl2h and soapcpp2. Run soapcpp2 on this file to generate the
(de)serialization code, which is stored in soapC.c[pp]. Include "soapH.h" in
your code to import these data type and function declarations. Only use the
soapcpp2-generated files in your project build. Do not include the wsdl2h-
generated .h file in your code.

XML content can be retrieved from:
  - a FILE* fd, using soap->recvfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->is = ...
  - a buffer, using the soap->frecv() callback

XML content can be stored to:
  - a FILE* fd, using soap->sendfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->os = ...
  - a buffer, using the soap->fsend() callback


@section ns Top-level root elements of schema "urn:UnimapSoapXml"

  - <ns:ImageSolveRequest> (use wsdl2h option -g to auto-generate)

  - <ns:ImageSolveResponse> (use wsdl2h option -g to auto-generate)

*/

/* End of unimap_service.h */