//
// Created by Cavan Fyans (cavan@sensum.co) at Sensum on 25/10/18.
//

#include <string>
#include <aws/core/auth/AWSCredentialsProvider.h>

namespace awsx {
  class Exception : public std::exception {
   protected:
    std::string m_message;

   public:
    Exception()
    {
    }

    Exception( const std::string & message )
        : m_message( message )
    {
    }

    const char * what() const noexcept override
    {
      return m_message.c_str();
    }
  };

  class CognitoAuth {
   protected:
    std::string m_clientId;
    std::string m_regionId;

    template <class TException, typename TResult> void ThrowIf( TResult & result )
    {
      if (!result.IsSuccess()) {
        throw TException( std::string( result.GetError().GetExceptionName().c_str() ) + ": " + result.GetError().GetMessage().c_str() );
      }
    }

   public:
    CognitoAuth( const std::string & regionId, const std::string & clientId )
        : m_regionId( regionId )
        , m_clientId( clientId )
    {
    }

    Aws::Auth::AWSCredentials Authenticate(
        const std::string & username,
        const std::string & password,
        const std::string & userPoolId,
        const std::string & identityPoolId );
  };
}