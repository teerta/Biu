using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text;

namespace Biu.Security
{
    /// <summary>
    /// Stamped password is a hashed password plus random stamp value.
    /// </summary>
    public class StampBasedUserPassword
    {
        public static void CreatePassword(ref string password, out string securityStamp)
        {
            try
            {
                string strHash = Hasher.HashText(HashMethod.SHA256, Encoding.UTF8, password);
                password = string.Empty;
                securityStamp = Guid.NewGuid().ToString();
                password = Hasher.HashText(HashMethod.MD5, Encoding.UTF8, strHash + securityStamp);
            }
            catch
            {
                throw;
            }
        }

        public static bool IsPasswordValid(ref string password, string securityStamp, string transformedPassword, bool clearPassword = true)
        {
            try
            {
                string strHash = Hasher.HashText(HashMethod.SHA256, Encoding.UTF8, password);
                if (clearPassword)
                {
                    password = string.Empty;
                }
               
                string strPwd = Hasher.HashText(HashMethod.MD5, Encoding.UTF8, strHash + securityStamp);

                return strPwd == transformedPassword;
            }
            catch
            {
                throw;
            }
        }
    }
}