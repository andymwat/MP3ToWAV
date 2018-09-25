using System;
using System.Collections.Generic;
using NLayer.NAudioSupport;
using NAudio.Wave;
using System.IO;
using System.Threading;
using System.Threading.Tasks;


namespace Mp3ToWAV
{
    class Program
    {
        public static bool MultiThreadedMode = true;
        public static int DebugLevel = 1; //0 = errors only, 1 = errors and info, 2 = verbose
        public static int Processors = Environment.ProcessorCount;
        public static int RunningThreads =0;
        public static List<Task> Tasks = new List<Task>();

        public static string[] ExpandFilePaths(string[] args)
        {
            var fileList = new List<string>();

            foreach (var arg in args)
            {
                var substitutedArg = Environment.ExpandEnvironmentVariables(arg);

                var dirPart = Path.GetDirectoryName(substitutedArg);
                if (dirPart.Length == 0)
                    dirPart = ".";

                var filePart = Path.GetFileName(substitutedArg);

                foreach (var filepath in Directory.GetFiles(dirPart, filePart))
                    fileList.Add(filepath);
            }

            return fileList.ToArray();
        }

        public static void Main(string[] args)
        {
            if (MultiThreadedMode && DebugLevel == 2)
            {
                Console.WriteLine("Found " + Processors + " CPUs");
            }
            Program prog = new Program();
            if (args.Length < 1)
            {
                Console.WriteLine("Requires at least 1 argument.");
            }
            else
            {

                string[] files = ExpandFilePaths(args);


                foreach (string file in files)
                {
                    //if not multithreading
                    //goes through files sequentially
                    if (!MultiThreadedMode)
                    {
                        if (File.Exists(file))
                        {
                            if (DebugLevel != 0)
                            {
                                Console.WriteLine("Converting file: " + file);
                            }

                            using (var reader = new Mp3FileReader(file, wf => new Mp3FrameDecompressor(wf)))
                            {
                                string outfile = Path.GetFileNameWithoutExtension(file) + ".wav";
                                WaveFileWriter.CreateWaveFile(outfile, reader);
                            }
                        }
                        else
                        {
                            Console.WriteLine("Could not find file " + file);
                        }
                    }

                    //if multithreading
                    //1 task for each file to convert.
                    else
                    {
                        
                        if (RunningThreads < Processors)
                        {
                            
                            //start new thread if not all cores are being used.
                            prog.StartNewThread(file);

                        }
                        else
                        {
                            while (RunningThreads < Processors)
                            {
                                //wait for thread to finish
                            }

                            prog.StartNewThread(file);
                        }
                    }
                }

                foreach (Task item in Tasks)
                {
                    if (DebugLevel == 2)
                    {
                        Console.WriteLine("Waiting for thread " + item.Id +  " to finish...");
                    }

                    item.Wait();
                }


            }

        }

        public  void StartNewThread(string file)
        {

            RunningThreads++;
            Action Action = () => ConverterWorker(file);
            Task task = new Task(Action);
            task.Start();
            Tasks.Add(task);
            Thread.Sleep(1);
            
        }

        public  void ConverterWorker(string file)
        {
            
            if (File.Exists(file))
            {
                if (DebugLevel != 0)
                {
                    Console.WriteLine("Converting file: " + file);
                }

                using (var reader = new Mp3FileReader(file, wf => new Mp3FrameDecompressor(wf)))
                {
                    string outfile = Path.GetFileNameWithoutExtension(file) + ".wav";
                    WaveFileWriter.CreateWaveFile(outfile, reader);
                }
            }
            else
            {
                Console.WriteLine("Could not find file " + file);
            }

            RunningThreads--;
        }
    }
}




