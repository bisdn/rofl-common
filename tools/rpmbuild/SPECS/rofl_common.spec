Name:		rofl-common
Version:	0.10.0
Release:	0%{?dist}
Summary:	revised openflow library
Epoch:		0

Group:		System Environment/Libraries
License:	Mozilla Public License Version 2.0, http://www.mozilla.org/MPL/2.0
URL:		https://github.com/bisdn/rofl-common
Source0:	rofl-common-%{version}.tar.gz

BuildRequires:	libconfig-devel openssl-devel
Buildroot: 	%{_tmppath}/%{name}-%{version}-root 
Requires:	libconfig openssl

%description
Revised OpenFlow Library (ROFL) version v0.10.0

%package        devel
Summary:        Header files and libraries for revised openflow library
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
Revised OpenFlow Library (ROFL) version v0.10.0 development files


%prep
%setup 


%build
sh autogen.sh
cd build/
../configure --prefix=/usr --disable-silent-rules --libdir=%{_libdir}
make %{?_smp_mflags} 


%install
rm -rf $RPM_BUILD_ROOT
cd build/
make install DESTDIR=$RPM_BUILD_ROOT


%post
cd %{_libdir}
/sbin/ldconfig
/bin/ln -s librofl_common.so.0 librofl_common.so
%postun
/sbin/ldconfig
/bin/rm %{_libdir}/librofl_common.so


%define _unpackaged_files_terminate_build 0 

%files
%defattr(-,root,root,-)
%{_libdir}/librofl_common.so.0.1.1
%{_libdir}/librofl_common.a
%{_libdir}/librofl_common.la
%doc


%files devel
%defattr(-,root,root,-)
%{_includedir}/rofl_common.h
%{_includedir}/rofl_common_conf.h
%{_includedir}/rofl
%{_libdir}/pkgconfig/rofl_common.pc
%doc


%clean
rm -rf $RPM_BUILD_ROOT 

%changelog
* Sun Nov 22 2015 Andreas Koepsel <andreas.koepsel@bisdn.de>
- build package for rofl-common-0.10.0

