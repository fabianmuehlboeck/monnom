using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Text;

namespace Nom
{
    public class Version : IComparable<Version>
    {
		public Version(short major = 1, short minor = 0, short revision = 0, short build = 0)
        {
			Major = major;
			Minor = minor;
			Revision = revision;
			Build = build;
        }
		public Version(String str)
        {
			string[] parts = str.Split('.');
			if(parts.Length<1||parts.Length>4)
            {
				throw new InvalidOperationException();
            }
			short major, minor, revision, build;
			if(!short.TryParse(parts[0], out major))
			{
				throw new InvalidOperationException();
			}
			if(parts.Length>1)
			{
				if (!short.TryParse(parts[1], out minor))
				{
					throw new InvalidOperationException();
				}
				if(parts.Length>2)
                {
					if (!short.TryParse(parts[2], out revision))
					{
						throw new InvalidOperationException();
					}
					if (parts.Length > 3)
					{
						if (!short.TryParse(parts[3], out build))
						{
							throw new InvalidOperationException();
						}
					}
					else
                    {
						build = 0;
                    }
				}
				else
                {
					revision = 0;
					build = 0;
				}
			}
			else
            {
				minor = 0;
				revision = 0;
				build = 0;
			}
			Major = major;
			Minor = minor;
			Revision = revision;
			Build = build;
		}
        public short Major { get; set; }
        public short Minor { get; set; }
        public short Revision { get; set; }
        public short Build { get; set; }

        public int CompareTo([AllowNull] Version other)
        {
            if(other==null)
            {
				return 1;
			}
			if (Major < other.Major)
			{
				return -1;
			}
			if (Major > other.Major)
			{
				return 1;
			}
			if (Minor < other.Minor)
			{
				return -1;
			}
			if (Minor > other.Minor)
			{
				return 1;
			}
			if (Revision < other.Revision)
			{
				return -1;
			}
			if (Revision > other.Revision)
			{
				return 1;
			}
			if (Build < other.Build)
			{
				return -1;
			}
			if (Build > other.Build)
			{
				return 1;
			}
			return 0;
		}

        public override string ToString()
        {
			return Major.ToString() + "." + Minor.ToString() + "." + Revision.ToString() + "." + Build.ToString();
        }

    }
}
