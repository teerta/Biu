using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.IO.Compression;

namespace Biu.IO
{
    public class GZip
    {
        public const string DefaultExt = ".gz";

        public GZip()
        {
        }

        public static bool Compress(Stream streamSource, Stream streamDest)
        {
            if (null == streamSource)
            {
                throw new ArgumentNullException("streamSource is null");
            }

            if (null == streamDest)
            {
                throw new ArgumentNullException("streamDest is null");
            }

            bool bRet = false;

            GZipStream gs = null;

            try
            {
                gs = new GZipStream(streamDest, CompressionMode.Compress);
                streamSource.CopyTo(gs);

                bRet = true;
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != gs)
                {
                    gs.Close();
                    gs = null;
                }
            }

            return bRet;
        }

        public static bool Decompress(Stream streamSource, Stream streamDest)
        {
            if (null == streamSource)
            {
                throw new ArgumentNullException("streamSource is null");
            }

            if (null == streamDest)
            {
                throw new ArgumentNullException("streamDest is null");
            }

            bool bRet = false;

            GZipStream gs = null;

            try
            {
                gs = new GZipStream(streamSource, CompressionMode.Decompress);
                gs.CopyTo(streamDest);

                bRet = true;
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != gs)
                {
                    gs.Close();
                    gs = null;
                }
            }

            return bRet;
        }

        public static bool Compress(Stream source, string dest)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(dest, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);

                return Compress(source, fs);
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

        public static bool Compress(string source, Stream dest)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                return Compress(fs, dest);
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

        public static bool Compress(string source, string dest)
        {
            FileStream fsSource = null;
            FileStream fsDest = null;

            try
            {
                fsSource = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                fsDest = new FileStream(dest, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);

                return Compress(fsSource, fsDest);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != fsSource)
                {
                    fsSource.Close();
                    fsSource = null;
                }

                if (null != fsDest)
                {
                    fsDest.Close();
                    fsDest = null;
                }
            }
        }

        public static bool Decompress(Stream source, string dest)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(dest, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);

                return Decompress(source, fs);
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

        public static bool Decompress(string source, Stream dest)
        {
            FileStream fs = null;

            try
            {
                fs = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                return Decompress(fs, dest);
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

        public static bool Decompress(string source, string dest)
        {
            FileStream fsSource = null;
            FileStream fsDest = null;

            try
            {
                fsSource = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                fsDest = new FileStream(dest, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);

                return Decompress(fsSource, fsDest);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != fsSource)
                {
                    fsSource.Close();
                    fsSource = null;
                }

                if (null != fsDest)
                {
                    fsDest.Close();
                    fsDest = null;
                }
            }
        }
    }
}
