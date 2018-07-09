Name:		rofl-common
Version:	0.13.1
Release:	1%{?dist}
Summary:	Revised OpenFlow Library

Group:		System Environment/Libraries
License:	MPLv2.0
URL:		https://github.com/bisdn/rofl-common
Source0:	rofl-common-%{version}.tar.gz

BuildRequires:	openssl-devel
BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	glog-devel
BuildRequires:	gflags-devel
Buildroot:	%{_tmppath}/%{name}-%{version}-root
Requires:	openssl
Requires:	glog

%description
Revised OpenFlow Library (ROFL) version %{version} - libraries

%package        devel
Summary:	Header files and libraries for revised openflow library
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}
Requires:	openssl-devel
Requires:	glog-devel
Requires:	gflags-devel

%description    devel
Revised OpenFlow Library (ROFL) version %{version} - development files

%prep
%setup -q

%build
sh ./autogen.sh
mkdir -p build
cd build/
../configure --prefix=/usr --disable-silent-rules --libdir=%{_libdir}
make %{?_smp_mflags} 

%install
rm -rf $RPM_BUILD_ROOT
cd build/
make install DESTDIR=$RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_libdir}/librofl_common.so.*
%doc README

%files devel
%defattr(-,root,root,-)
%{_includedir}/rofl_common.h
%{_includedir}/rofl_common_conf.h
%{_includedir}/rofl
%{_libdir}/librofl_common.a
%{_libdir}/librofl_common.la
%{_libdir}/librofl_common.so
%{_libdir}/pkgconfig/rofl_common.pc

%clean
rm -rf $RPM_BUILD_ROOT 

%changelog
* Wed Jul 04 2018 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.13.1
- [+] added experimental matches from OFDPA-2:
      VRF, OVID, ALLOW_VLAN_TRANSLATION, ACTSET_OUTPUT
- [B] fixed invalid length fields for experimental OXM matches
- [B] ensure all padding fields are set to value 0

* Mon Jul 02 2018 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.13.0
- [+] thread pool

* Mon Feb 19 2018 Tobias Jungel <tobias.jungel@bisdn.de> - v0.12.2
- [B] cofport_stats default port_no fixed
- [O] improved testing on travis

* Wed Sep 13 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.12.1
- [A] queue_type be public enum
- [B] fix ctimespec operator
 
* Fri Jul 21 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.12.0
- [+] crofsock: revised tx-queueing strategy (default txqueue size limited
      to 128 messages), API changes to crofctl and crofdpt: all transmission
      methods return the queueing result when sending an openflow message:
      MSG_QUEUED, MSG_QUEUED_CONGESTION, MSG_QUEUEING_FAILED_*
- [B] crofchantest: thread sanitizer friendly
- [B] build-system: update OpenSSL dependency
- [A] typos in functions fixed


* Thu Jun 22 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.11.3
- [B] remove ambiguous get_time

* Thu Jun 15 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.11.2
- [B] cofmsg: fix copy of cofmsg_features_reply
- [+] crofconn: handle_rx_messages optimization

* Thu Apr 13 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.11.1
- [+] coxmatch: added outputter for ppp(oe) experimental oxmatches
- [+] crofconn: hello timeout values decreased
- [B] crofconn: call handle_closed in case echo_request_expired
- [+] use of __FUNCTION__ instead of __PRETTY_FUNCTION__ is sufficient
- [O] Documentation updated wrt. dependencies
- [B] crofbase: prevent shadowing of cthread callbacks

* Mon Mar 13 2017 Tobias Jungel <tobias.jungel@bisdn.de> - v0.11.0
- [B] made ofp_version atomic
- [B] multiple thread related issues found with tsan
- [B] crofsock: txqueue order of barrier requests
- [+] crofbase: allow set_ctl without creation of a new instance
- [B] ctimespec: invalid calculation and data race fixed
- [+] travis: print log of failed tests
- [+] log exceptions caught in thread
- [+] cdptid and cdpid are now basic types
- [+] cthread: timer cleanup
- [B] tests are now included in dist release
- [+] rofl journal replaced with glog
- [+] reset txbuffer before sending to comply with ovs and ofagent
- [+] clang format added
- [+] cthread: cleanup locking and api
- [B] heap-use-after-free in crofconn
- [B] crofbase: return if accept fails with EAGAIN
- [+] cthread: set name of thread
- [B] crofbase: heap-use-after-free fixed
- [B] crofsock: heap-use-after-free fixed
- [B] cthread: use pthread_join so finalize a thread
- [B] debug log of OF version number fixed

* Thu Jun 30 2016 Tobias Jungel <tobias.jungel@bisdn.de> - v0.10.9
- [+] fixed issues with compilation under GCC6
- [+] Extended behaviour of method rofl::crofbase::set_dpt()

* Tue May 10 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.8
- [B] crofdpt: do not automatically assign xid to experimenter messages

* Mon Apr 11 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.7
- [B] crofsock: fixed bug in congestion management
- [+] cthread: allow level-triggered file descriptors

* Fri Apr 08 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.6
- [B] crofsock: prevent infinite loop
      In case txqueues are not empty tx is rescheduled. The
      flag to do so was not cleared and this would cause an
      infinite loop. Reset the flag to prevent this.
- [B] crofsock: crofsock: set tx_fragment_pending in tx saturation
      A packet in the txbuffer would not be sent in case tx
      was saturated tx_fragment_pending is set to resend the
      packet, that was already cleared from the txqueues.
- [B] crofsock: txqueue_size_tx_threshold checking fixed
      If txqueue_pending_pkts == 1, the calculation of
      txqueue_size_tx_threshold might lead to a never cleared
      FLAG_TX_BLOCK_QUEUEING flag.
- [B] cthread: don't throw in destructor
- [B] crofchantest: fix increments done in threads
      num_of_ctl_established and num_of_accepts are incremented
      from diffrent threads, hence the test randomly fails or
      succeeds.
- [B] crofsocktest: fix timeout
      timeout was already -1 in CPPUNIT_ASSERT
- [K] TLS support broken
      crofsocktest: disabling TLS test
- [B] cthread: fix edge-triggered behaviour of epoll
      pipe still uses level-triggered scheme
- [B] crofsock: don't let handle_write_event run at state STATE_CLOSED
- [B] crofsock: do not wakeup tx in case its blocked
- [B] crofconn: fixed potential race condition during connection
      establishment (HELLO negotiation)
- [B] crofsock: add rxthread wakeup handler
      call recv_message() after connect/EINPROGRESS continuation
      in state STATE_TCP_CONNECTING
- [B] crofsock: fixed bug in listening mode

* Wed Apr 06 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.5
- [B] version is broken, do not use

* Wed Mar 02 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.4
- crofchan: corrected bugfix from v0.10.3 in handle_closed()

* Tue Mar 01 2016 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.3
- added cclockid class to prevent invalid use of class ctimespec
- crofsock: use SO_LINGER option for coordinated shutdown
- crofsock: make backlog for listening mode configurable, defaults to 64 established TCP connections
- crofconn: fixed potential race condition causing a Hello expiration
- crofchan: send CLOSE notification to application when replacing an existing connection object (restart)
- crofchan: fixed bug in handle_closed(): conn was not removed from crofchan::conns map leading to a double free

* Thu Feb 25 2016 Tobias Jungel <tobias.jungel@bisdn.de> - v0.10.2
- [+] more verbose exception handling
- [+] asserts to thread handlers
- [+] cthread: ignore signals in event loop
- [B] coxmatches::copy_matches(): copy experimental matches properly

* Fri Jan 29 2016 xxx - v0.10.1-0
- todo

* Sun Nov 22 2015 Andreas Koepsel <andreas.koepsel@bisdn.de> - v0.10.0-0
- build package for rofl-common-0.10.0

