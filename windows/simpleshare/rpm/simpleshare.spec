# This is a spec file SimpleShare

%define _topdir	 	/home/local/rpmbuild
%define name		simpleshare
%define release		1
%define version 	1.0.32
%define buildroot       %{_topdir}/BUILD/%{name}-%{version}

BuildRoot:              %{buildroot}
Summary: 		BOX SimpleShare
License: 		CC-BY-ND
Name: 			%{name}
Version: 		%{version}
Release: 		%{release}
Source: 		simpleshare-%{version}.tar.gz
Prefix: 		/usr
Group: 			Applications/Internet
URL:			http://www.box.net/simpleshare

%description

Use SimpleShare to quickly share images and other
files directly from your desktop. This product is free
to use and works with any Box account type.

%prep
%setup -q

%build
/usr/lib/qt4/bin/qmake simpleshare.pro
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
install simpleshare $RPM_BUILD_ROOT/usr/bin/simpleshare
cp %{_topdir}/BUILD/%{name}-%{version}/rpm/box-simpleshare.desktop $RPM_BUILD_ROOT/usr/share/applications/box-simpleshare.desktop

%preun
rm /usr/bin/simpleshare
rm /usr/share/applications/box-simpleshare.desktop

%files
%defattr(-,root,root)
/usr/bin/simpleshare
/usr/share/applications/box-simpleshare.desktop
