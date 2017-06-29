using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Cryptography;
using System.IO;

namespace Biu.Security
{
    public static class Hasher
    {
        public static bool CompareResult(HashMethod method, string file, string hash)
        {
            try
            {
                if (!File.Exists(file))
                {
                    return false;
                }

                string strResult = string.Empty;
                switch(method)
                {
                    case HashMethod.SHA256:
                        strResult = SHA256(file);
                        break;

                    case HashMethod.SHA512:
                        strResult = SHA512(file);
                        break;

                    case HashMethod.MD5:
                        strResult = MD5(file);
                        break;
                }

                StringComparer sc = StringComparer.OrdinalIgnoreCase;

                return 0 == sc.Compare(strResult, hash);
            }
            catch
            {
                throw;
            }
        }

        public static bool CompareResult(HashMethod method, Stream stream, string hash)
        {
            try
            {
                string strResult = string.Empty;
                switch (method)
                {
                    case HashMethod.SHA256:
                        strResult = SHA256(stream);
                        break;

                    case HashMethod.SHA512:
                        strResult = SHA512(stream);
                        break;

                    case HashMethod.MD5:
                        strResult = MD5(stream);
                        break;
                }

                StringComparer sc = StringComparer.OrdinalIgnoreCase;

                return 0 == sc.Compare(strResult, hash);
            }
            catch
            {
                throw;
            }
        }

        public static bool Compare(HashMethod method, string file1, string file2)
        {
            try
            {
                if (!File.Exists(file1) || !File.Exists(file2))
                {
                    return false;
                }

                string strHash1 = string.Empty;
                string strHash2 = string.Empty;
                switch (method)
                {
                    case HashMethod.SHA256:
                        strHash1 = SHA256(file1);
                        strHash2 = SHA256(file2);
                        break;

                    case HashMethod.SHA512:
                        strHash1 = SHA512(file1);
                        strHash2 = SHA512(file2);
                        break;

                    case HashMethod.MD5:
                        strHash1 = MD5(file1);
                        strHash2 = MD5(file2);
                        break;
                }

                StringComparer sc = StringComparer.OrdinalIgnoreCase;

                return 0 == sc.Compare(strHash1, strHash2);
            }
            catch
            {
                throw;
            }
        }

        public static bool Compare(HashMethod method, Stream stream1, Stream stream2)
        {
            try
            {
                string strHash1 = string.Empty;
                string strHash2 = string.Empty;
                switch (method)
                {
                    case HashMethod.SHA256:
                        strHash1 = SHA256(stream1);
                        strHash2 = SHA256(stream2);
                        break;

                    case HashMethod.SHA512:
                        strHash1 = SHA512(stream1);
                        strHash2 = SHA512(stream2);
                        break;

                    case HashMethod.MD5:
                        strHash1 = MD5(stream1);
                        strHash2 = MD5(stream2);
                        break;
                }

                StringComparer sc = StringComparer.OrdinalIgnoreCase;

                return 0 == sc.Compare(strHash1, strHash2);
            }
            catch
            {
                throw;
            }
        }

        public static bool Compare(HashMethod method, string file, Stream stream)
        {
            try
            {
                if (!File.Exists(file))
                {
                    return false;
                }

                string strHash1 = string.Empty;
                string strHash2 = string.Empty;
                switch (method)
                {
                    case HashMethod.SHA256:
                        strHash1 = SHA256(file);
                        strHash2 = SHA256(stream);
                        break;

                    case HashMethod.SHA512:
                        strHash1 = SHA512(file);
                        strHash2 = SHA512(stream);
                        break;

                    case HashMethod.MD5:
                        strHash1 = MD5(file);
                        strHash2 = MD5(stream);
                        break;
                }

                StringComparer sc = StringComparer.OrdinalIgnoreCase;

                return 0 == sc.Compare(strHash1, strHash2);
            }
            catch
            {
                throw;
            }
        }

        public static string SHA256(Stream stream)
        {
            if (null == stream)
            {
                throw new ArgumentNullException("stream is null");
            }

            string strRet = string.Empty;

            SHA256 sha = null;
            try
            {
                sha = new SHA256Managed();

                stream.Seek(0, SeekOrigin.Begin);

                byte[] buff = sha.ComputeHash(stream);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != sha)
                {
                    sha.Dispose();
                    sha = null;
                }
            }

            return strRet;
        }

        public static string SHA256(string file)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                return SHA256(fs);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != fs)
                {
                    fs.Close();
                    fs = null;
                }
            }
        }

        public static string SHA512(Stream stream)
        {
            if (null == stream)
            {
                throw new ArgumentNullException("stream is null");
            }

            string strRet = string.Empty;

            SHA512 sha = null;
            try
            {
                sha = new SHA512Managed();

                stream.Seek(0, SeekOrigin.Begin);

                byte[] buff = sha.ComputeHash(stream);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != sha)
                {
                    sha.Dispose();
                    sha = null;
                }
            }

            return strRet;
        }

        public static string SHA512(string file)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                return SHA512(fs);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != fs)
                {
                    fs.Close();
                    fs = null;
                }
            }
        }

        public static string MD5(Stream stream)
        {
            if (null == stream)
            {
                throw new ArgumentNullException("stream is null");
            }

            string strRet = string.Empty;

            MD5 md5 = null;
            try
            {
                md5 = System.Security.Cryptography.MD5.Create();

                stream.Seek(0, SeekOrigin.Begin);

                byte[] buff = md5.ComputeHash(stream);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != md5)
                {
                    md5.Dispose();
                    md5 = null;
                }
            }

            return strRet;
        }

        public static string MD5(string file)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                return MD5(fs);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != fs)
                {
                    fs.Close();
                    fs = null;
                }
            }
        }

        public static string HashTextUTF8(HashMethod method, string content)
        {
            try
            {
                return HashText(method, Encoding.UTF8, content);
            }
            catch
            {
                throw;
            }
        }

        public static string HashText(HashMethod method, Encoding encoding, string content)
        {
            try
            {
                switch (method)
                {
                    case HashMethod.MD5:
                        return MD5Text(encoding, content);

                    case HashMethod.SHA256:
                        return SHA256Text(encoding, content);

                    case HashMethod.SHA512:
                        return SHA512Text(encoding, content);

                    default:
                        return null;
                }
            }
            catch
            {
                throw;
            }
        }

        private static string MD5Text(Encoding encoding, string content)
        {
            if (string.IsNullOrEmpty(content))
            {
                throw new ArgumentNullException("content is null");
            }

            string strRet = string.Empty;

            MD5 md5 = null;
            try
            {
                md5 = System.Security.Cryptography.MD5.Create();
                byte[] buffContent = encoding.GetBytes(content);

                byte[] buff = md5.ComputeHash(buffContent);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != md5)
                {
                    md5.Dispose();
                    md5 = null;
                }
            }

            return strRet;
        }

        private static string SHA256Text(Encoding encoding, string content)
        {
            if (string.IsNullOrEmpty(content))
            {
                throw new ArgumentNullException("content is null");
            }

            string strRet = string.Empty;

            SHA256 sha = null;
            try
            {
                sha = new SHA256Managed();
                byte[] buffContent = encoding.GetBytes(content);

                byte[] buff = sha.ComputeHash(buffContent);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != sha)
                {
                    sha.Dispose();
                    sha = null;
                }
            }

            return strRet;
        }

        private static string SHA512Text(Encoding encoding, string content)
        {
            if (string.IsNullOrEmpty(content))
            {
                throw new ArgumentNullException("content is null");
            }

            string strRet = string.Empty;

            SHA512 sha = null;
            try
            {
                sha = new SHA512Managed();
                byte[] buffContent = encoding.GetBytes(content);

                byte[] buff = sha.ComputeHash(buffContent);
                strRet = FormatByteArray(buff);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != sha)
                {
                    sha.Dispose();
                    sha = null;
                }
            }

            return strRet;
        }

        private static string FormatByteArray(byte[] buff)
        {
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < buff.Length; i++)
            {
                sb.Append(string.Format("{0:X2}", buff[i]));
            }

            return sb.ToString();
        }
    }
}
