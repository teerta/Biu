using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Biu.IO
{
    public class FileUtil
    {
        /// <summary>
        /// To ensure that file's path is unique.
        /// Generate a new filename like "%Path%\File[nnnnnn].xxx", where n's range is from 1 to 999999 when file exits.
        /// </summary>
        /// <param name="dir"></param>
        /// <param name="path"></param>
        /// <returns></returns>
        public static bool EnsureFile(string dir, ref string path)
        {
            string strFile = string.Format("{0}\\{1}", dir, path);
            if (!File.Exists(strFile))
            {
                path = strFile;
                return true;
            }

            string strExt = Path.GetExtension(path);
            string strName = Path.GetFileNameWithoutExtension(path);
            Random rand = new Random();

            do
            {
                strFile = string.Format("{0}\\{1}[{2}]{3}", dir, strName, rand.Next(1, 999999), strExt);
            } while (File.Exists(strFile));

            path = strFile;

            return true;
        }

        /// <summary>
        /// Ensure each directory part in path is valid (exist).
        /// </summary>
        /// <param name="path"></param>
        /// <param name="includeFile">To determine if there is a file name in path</param>
        /// <param name="ignoreFirst">Ignore 1st part in the path (something like c:\)</param>
        public static void EnsureDirsAlongPath(string path, bool includeFile = true, bool ignoreFirst = true)
        {
            try
            {
                string strDir = null;
                var sb = new StringBuilder();
                string[] parts = path.Split(new char[] { Path.DirectorySeparatorChar });
                int nStart = 0;
                int nEnd = includeFile ? parts.Length - 1 : parts.Length;
                if (ignoreFirst)
                {
                    nStart = 1;
                    sb.Append(parts[0]).Append(Path.DirectorySeparatorChar);
                }
                
                for (int i = nStart; i < nEnd; i++)
                {                    
                    //for (int j = nStart; j <= i; j++)
                    //{
                    //    sb.Append(parts[j]).Append(Path.DirectorySeparatorChar);
                    //}
                    sb.Append(parts[i]).Append(Path.DirectorySeparatorChar);
                    strDir = sb.ToString();
                    strDir = strDir.Substring(0, strDir.Length - 1);
                    if (!Directory.Exists(strDir))
                    {
                        Directory.CreateDirectory(strDir);
                    }
                }
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Ensure each dir in dirs exists (absolute path, recursive).
        /// </summary>
        /// <param name="dirs"></param>
        public static void EnsureDirs(IEnumerable<string> dirs)
        {
            try
            {
                foreach (var dir in dirs)
                {
                    EnsureDirsAlongPath(dir, false);
                }
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Copy directory (absolute path).
        /// </summary>
        /// <param name="srcDir"></param>
        /// <param name="destDir"></param>
        /// <param name="recursive"></param>
        public static void CopyDir(string srcDir, string destDir, bool recursive = false)
        {
            try
            {
                if (!Directory.Exists(srcDir))
                {
                    throw new DirectoryNotFoundException(srcDir);
                }

                EnsureDirsAlongPath(destDir, false);
                if (recursive)
                {
                    string strSubDestDir = null;                    
                    foreach (var subDir in Directory.EnumerateDirectories(srcDir))
                    {
                        strSubDestDir = Path.Combine(destDir, subDir.Substring(subDir.LastIndexOf(Path.DirectorySeparatorChar) + 1));
                        EnsureDirsAlongPath(strSubDestDir, false);
                        CopyDir(subDir, strSubDestDir, true);
                    }
                }                

                foreach (var file in Directory.EnumerateFiles(srcDir))
                {
                    File.Copy(file, Path.Combine(destDir, Path.GetFileName(file)), true);
                }
            }
            catch
            {
                throw;
            }
        }
    }
}
