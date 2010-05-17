%define major_version 10.05.0

%define min_glib2 2.14
%define min_libxml2 2.6
%define devel_requires glib2-devel >= %{min_glib2}, libxml2-devel >= %{min_libxml2}, libgda-devel, dbus-devel, dbus-glib-devel

Name:           midgard2-core
Version:        %{major_version}
Release:        1%{?dist}
Summary:        Midgard core library and tools

Group:          System Environment/Base
License:        LGPLv2+
URL:            http://www.midgard-project.org/
Source0:        %{url}download/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  pkgconfig >= 0.9.0
BuildRequires:  %{devel_requires}
BuildRequires:  pam-devel

Requires:       glib2 >= %{min_glib2}, libxml2 >= %{min_libxml2}
Requires(post): /bin/ls, /bin/grep

%description                
Midgard is a persistent storage framework built for the replicated
world. It enables developers build applications that have their data in
sync between the desktop, mobile devices and web services. It also
allows for easy sharing of data between users.

Midgard does this all by building on top of technologies like GLib, 
Libgda and D-Bus. It provides developers with object-oriented 
programming interfaces for C, PHP and Python.

This package provides the core C library and tools of the Midgard 
framework. The library allows Midgard applications to access the Midgard 
database using a set of database-independent functions. The library also 
does user authentication and privilege handling.


%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       %{devel_requires}

%description devel
The %{name}-devel package contains libraries and header files for 
developing applications that use %{name}.


%prep
%setup -q


%build
%configure --disable-static
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $(dirname $RPM_BUILD_ROOT)
mkdir $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'


%clean
rm -rf $RPM_BUILD_ROOT


%post
/sbin/ldconfig
/bin/ls -1 %{_sysconfdir}/midgard2/conf.d/ | /bin/grep -v ^midgard\.conf\.example$ | while read file; do
    %{_bindir}/midgard2-schema "$file" > /dev/null 2>&1
done
exit 0

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/*.so.*
%dir %{_sysconfdir}/midgard2
%dir %{_sysconfdir}/midgard2/conf.d
%config(noreplace,missingok) %{_sysconfdir}/midgard2/conf.d/*
%config(noreplace) %{_sysconfdir}/dbus-1/system.d/*
%{_bindir}/*
%{_mandir}/man1/*
%dir %{_datadir}/midgard2
%{_datadir}/midgard2/*

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/midgard2
%dir %{_includedir}/midgard2/midgard
%{_includedir}/midgard2/midgard/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/*


%changelog
* Tue Oct 27 2009 Jarkko Ala-Louvesniemi <jval@puv.fi> 9.03.99-1
- Renamed package from midgard-core to midgard2-core
- Removed OpenSSL and MySQL from development requirements
- Added Libgda to development requirements
- Removed Flex from build requirements
- Updated package description to include Python and Libgda
- Changed midgard to midgard2 in directory and file names
- Include files are now in midgard2/midgard
- Use macros for glib2 and libxml2 minimum version requirements
- GLib requirement is now >= 2.14 (tests require >= 2.16)
- Added dbus-1/system.d/* to configuration files

* Thu Sep 17 2009 Jarkko Ala-Louvesniemi <jval@puv.fi> 8.09.5-2
- Added missingok for the example configuration file(s)

* Thu Jul 16 2009 Jarkko Ala-Louvesniemi <jval@puv.fi> 8.09.5-1
- Initial package using the Fedora spec file template.
