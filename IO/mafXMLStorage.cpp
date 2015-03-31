/*=========================================================================

 Program: MAF2
 Module: mafXMLStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//#include "mafIncludeWX.h" // to be removed

#include "mafXMLStorage.h"
#include "mafXMLElement.h"
#include "mafXMLString.h"
#include "mafStorable.h"
#include "mmuDOMTreeErrorReporter.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
// Xerces-C specific
#include "mmuXMLDOM.h"
#include "mmuXMLDOMElement.h"
#include <xercesc/framework/LocalFileInputSource.hpp>
// required by error handlers
//#include <xercesc/dom/DOMErrorHandler.hpp>
//#include <xercesc/dom/DOMError.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include <assert.h>

#ifndef MAF_USE_WX
#error "XML Storage cannot be compiled without wxWidgets"
#endif

//------------------------------------------------------------------------------
// mafXMLStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafXMLParser)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafXMLParser::mafXMLParser()
//------------------------------------------------------------------------------
{
  m_DOM = new mmuXMLDOM;
}

//------------------------------------------------------------------------------
mafXMLParser::~mafXMLParser()
//------------------------------------------------------------------------------
{
  cppDEL(m_DOM);
}

//------------------------------------------------------------------------------
int mafXMLParser::InternalStore()
//------------------------------------------------------------------------------
{
  int errorCode=0;
  // initialize the XML library
  try
  {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
  }

  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
  {
      mafErrorMessageMacro("Error during Xerces-C Initialization.\nException message:" << mafXMLString(toCatch.getMessage()));      
      return MAF_ERROR;
  }

  // get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
  m_DOM->m_XMLImplement = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(mafXMLString("LS"));

  if (m_DOM->m_XMLImplement)
  {
    m_DOM->m_XMLSerializer = ( (XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)m_DOM->m_XMLImplement )->createLSSerializer();

    m_DOM->m_XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(m_URL);

    // set user specified end of line sequence and output encoding
    m_DOM->m_XMLSerializer->setNewLine( mafXMLString("\r") );

    // set serializer features 
    XERCES_CPP_NAMESPACE_QUALIFIER DOMConfiguration  *config = m_DOM->m_XMLSerializer->getDomConfig();
    config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTSplitCdataSections, false);
    config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, false);
    config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);
  	config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTBOM, false);

    try
    {
      // create a document
      m_DOM->m_XMLDoc = m_DOM->m_XMLImplement->createDocument( NULL, mafXMLString(m_FileType), NULL ); // NO URI and NO DTD
      if (m_DOM->m_XMLDoc)
      {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMLSOutput *theOutputDesc = m_DOM->m_XMLImplement->createLSOutput();
        // output related nodes are prefixed with "svg"
        // to distinguish them from input nodes.
 	      theOutputDesc->setEncoding( mafXMLString("UTF-8") );
        theOutputDesc->setByteStream(m_DOM->m_XMLTarget);
	      m_DOM->m_XMLDoc->setXmlStandalone(true);
	      m_DOM->m_XMLDoc->setXmlVersion( mafXMLString("1.0") );

        // extract root element and wrap it with an mafXMLElement object
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
        assert(root);
        mafStorageElement *documentElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

        // attach version attribute to the root node
        documentElement->SetAttribute("Version",m_Version);
      
        // call Store function of the m_Document object. The root is passed
        // as parent the DOM root element. A tree root is usually a special
        // kind of object and can decide to store itself in the root
        // object itself, or below it as it happens for other nodes.
        assert(m_Document);
        m_Document->Store(documentElement);

        // write the tree to disk
        m_DOM->m_XMLSerializer->write(m_DOM->m_XMLDoc, theOutputDesc);

        // destroy all intermediate objects
        theOutputDesc->release();
        cppDEL (documentElement);  
        cppDEL (m_DOM->m_XMLTarget);
        cppDEL (m_DOM->m_XMLDoc);
        errorCode=0;
      }    
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
    {
      mafErrorMessageMacro( "XML error, DOMException code is:  " << e.code );
      errorCode = 2;
    }
    catch (...)
    {
       mafErrorMessage("XML error, an error occurred creating the XML document!");
       errorCode = 3;
    }

    cppDEL (m_DOM->m_XMLTarget);
    cppDEL (m_DOM->m_XMLSerializer);
    cppDEL (m_DOM->m_XMLDoctype);
  }
  else
  {
    // implementation retrieve failed
    mafErrorMessage("Requested XML implementation is not supported");
    errorCode = 1;
  }
 
  // terminate the XML library
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

  return errorCode;
}

//------------------------------------------------------------------------------
int mafXMLParser::InternalRestore()
//------------------------------------------------------------------------------
{
  assert (m_Document);

  if (!m_Document)
    return MAF_ERROR;
  
  int errorCode=0;
  
  // initialize the XML library
  try
  {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
  }

  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
  {
    mafErrorMessageMacro( "Error during Xerces-C Initialization.\nException message:" <<mafXMLString(toCatch.getMessage()));
    return MAF_ERROR;
  }
  
  //
  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.
  //
  m_DOM->m_XMLParser = new XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

  if (m_DOM->m_XMLParser)
  {
    m_DOM->m_XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
    m_DOM->m_XMLParser->setDoNamespaces(false);
    m_DOM->m_XMLParser->setDoSchema(false);
    m_DOM->m_XMLParser->setCreateEntityReferenceNodes(false);

    mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
    m_DOM->m_XMLParser->setErrorHandler(errReporter);

    {
      try
      {
        m_DOM->m_XMLParser->parse(m_URL);
        int errorCount = m_DOM->m_XMLParser->getErrorCount(); 

        if (errorCount != 0)
        {
          // errors while parsing...
          mafErrorMessage("Errors while parsing XML file");
          errorCode = IO_XML_PARSE_ERROR;
        }
        else
        {
          // extract the root element and wrap inside a mafXMLElement
          m_DOM->m_XMLDoc = m_DOM->m_XMLParser->getDocument();
          XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
          assert(root);
          mafStorageElement *documentElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

          if (m_FileType == documentElement->GetName())
          {
            mafString docVersion;
            if (documentElement->GetAttribute("Version",docVersion))
            {
              double doc_version_f = atof(docVersion);
              double my_version_f = atof(m_Version);
            
              if (my_version_f <= doc_version_f)
              {
                // Start tree restoring from root node
                if (m_Document->Restore(documentElement) != MAF_OK)
                  errorCode = IO_RESTORE_ERROR;
              }
              else
              {
                // Paolo 30-11-2007: due to changes on name for mafVMEScalar (to mafVMEScalarMatrix)
                if (doc_version_f < 2.0)
                {
                  mafErrorMacro("XML parsing error: wrong file version v"<<docVersion.GetCStr()<<", should be > v"<<m_Version.GetCStr());
                  errorCode = IO_WRONG_FILE_VERSION;
                }
                else
                {
                  // Upgrade document to the actual version
                  documentElement->SetAttribute("Version", my_version_f);
                  m_NeedsUpgrade = true;
                  if (m_Document->Restore(documentElement) != MAF_OK)
                    errorCode = IO_RESTORE_ERROR;
                }
              }
            }
          }
          else
          {
            mafErrorMacro("XML parsing error: wrong file type, expected \""<<m_FileType<<"\", found "<<documentElement->GetName());
            errorCode = IO_WRONG_FILE_TYPE;
          }
          
          // destroy the root XML element
          cppDEL(documentElement);
        }
      }

      catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& e)
      {
        mafString err;
        err << "An error occurred during XML parsing.\n Message: " << mafXMLString(e.getMessage());
        mafErrorMessage(err);
        errorCode = IO_XML_PARSE_ERROR;
      }

      catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
      { 
        mafString err;
        err << "DOM-XML Error while parsing file '" << m_URL << "'\n";
        err << "DOMException code is: " << mafString(e.code);

        if (e.getMessage())
          err << "DOMException msg is: " << mafXMLString(e.getMessage());
      
        mafErrorMessage(err);
        errorCode = IO_DOM_XML_ERROR;
      }

      /*catch (const SAXException& e)
      {
        mafString err;
        err << "SAX-XML Error while parsing file: '" << m_ParserURL << "'\n";
        err << "SAXException msg is: " << mafXMLString(e.getMessage());
        mafErrorMessage(err);
      }*/

      catch (...)
      {
        mafErrorMessage("An error occurred during XML parsing");
        errorCode = IO_XML_PARSE_ERROR;
      }
    }

    cppDEL (errReporter);
    cppDEL (m_DOM->m_XMLParser);
  }
  else
  {
    // parser allocation error
    mafErrorMessage("Failed to allocate XML parser");
    errorCode = IO_XML_PARSER_INTERNAL_ERROR;
  }

  // terminate the XML library
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
  
  if (GetErrorCode()==0)
    SetErrorCode(errorCode);

  return GetErrorCode();
}
