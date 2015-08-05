#!/usr/bin/env perl 

use v5.14;
use strict;
use warnings;
use IO::Socket::INET;
use LWP::UserAgent;

use constant LOCALHOST => "192.168.1.80";
use constant WEBSERVER => "http://192.168.1.80:3000/drive";

$| = 1;

my ($socket,$client_socket);
my ($peeraddress,$peerport);

$socket = new IO::Socket::INET (
        LocalHost => LOCALHOST,
        LocalPort => '3000',
        Proto     => 'tcp',
        Listen    => 5,
        Reuse     => 1,
        Timeout   => 1,
) or die "ERROR in Socket Creation : $!\n";

print "SERVER Waiting for client connection on port 3001\n";

while(1)
{
    say "Wait accept";
    $client_socket = $socket->accept();

    if ($client_socket) {
        my $peerport = $client_socket->peerport();
        my $peerhost = $client_socket->peerhost();

        say "p_port : $peerport\n p_host : $peerhost\n";

        my $data = 'start';
        while ($data ne 'close') {
            $data = '';
            $client_socket->recv($data,1024);

            my $ua = LWP::UserAgent->new;
            my $server_endpoint = WEBSERVER;

            $ua->post($server_endpoint, { course => $data });

            say $data;
            print $client_socket $data;
        }

        print $client_socket "server closed";
    }
}

$socket->close();
