%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}
%define packagename curses

Name:          tcl-curses
Summary:       A "minimalist" tcl package for interfacing to curses
Version:       0.8.0
Release:       0
License:       TCL
Group:         Development/Libraries/Tcl
Source:        %{name}-%{version}.tar.gz
URL:           https://github.com/ray2501/tcl-curses
BuildRequires: autoconf
BuildRequires: make
BuildRequires: tcl-devel >= 8.4
BuildRequires: ncurses-devel
Requires:      tcl >= 8.4
BuildRoot:     %{buildroot}

%description
It is a "minimalist" tcl package for interfacing to curses.

%prep
%setup -q -n %{name}-%{version}

%build
./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{tcl_archdir}/%{packagename}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%{tcl_archdir}
