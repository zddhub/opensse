#!/usr/bin/ruby

require 'optparse'

argv_size = ARGV.size
options = {}

optparse = OptionParser.new do |opts|
    opts.banner = "Usage: generate_filelist -d rootdir -f filter -o filelist [-n sample_num]"
    opts.on('-d', '--rootdir rootdir', 'file root directory') { |v| options[:rootdir] = v }
    opts.on('-f', '--filter filter', 'file name filter') { |v| options[:filter] = v }
    opts.on('-o', '--filelist filelist', 'output file name') { |v| options[:filelist] = v }
    opts.on('-n', '--sample_num sample_num', 'sample num') { |v| options[:sample_num] = v }
end

begin
    optparse.parse!
rescue OptionParser::InvalidArgument, OptionParser::InvalidOption, OptionParser::MissingArgument
    puts $!.to_s
    puts optparse
    exit
end

if argv_size != 6 && argv_size != 8
    puts optparse
end

Dir.chdir(options[:rootdir])
lists =  Dir.glob("**/"+options[:filter])

file = File.new(options[:filelist], "w")
lists.each do |image|
    file.puts image
end

file.close

