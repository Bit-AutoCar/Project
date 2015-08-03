#!/usr/bin/env perl 

use v5.14;
use strict;
use warnings;
use IO::Socket::INET;
use IO::Socket::Timeout;
use LWP::UserAgent;

$| = 1;

my ($socket,$client_socket);
my ($peeraddress,$peerport);

$socket = new IO::Socket::INET (
        LocalHost => '192.168.1.3',
        LocalPort => '3000',
        Proto     => 'tcp',
        Listen    => 5,
        Reuse     => 1,
        Timeout   => 3
) or die "ERROR in Socket Creation : $!\n";

print "SERVER Waiting for client connection on port 3000\n";

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
            my $server_endpoint = "192.168.1.31:3000/drive";

# set custom HTTP request header fields
            my $req = HTTP::Request->new(POST => $server_endpoint);
            $req->header('content-type' => 'application/json');
            $req->header('x-auth-token' => 'kfksj48sdfj4jd9d');

# add POST data to HTTP request body
            my $post_data = "{ name: $data }";
            $req->content($post_data);

            say $data;
            print $client_socket $data;
        }

        print $client_socket "server closed";
    }
}

$socket->close();
