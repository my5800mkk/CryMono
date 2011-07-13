﻿using System;
using System.Reflection;
using System.IO;
using System.Diagnostics;
namespace Cemono
{
    public class Manager
    {
        #region Fields
        private ConsoleRedirector _consoleRedirector;
        private ConsoleTraceListener _consoleTraceListener;
        private AppDomain _gameDomain;
        #endregion

        #region Properties

        #endregion

        #region Constructor(s)
        public Manager()
        {
            InitializeConsoleRedirect();

            // Catch unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;

            // Initialize game appdomain
            InitializeGameDomain();
        }

        #endregion

        #region Methods
        private void InitializeConsoleRedirect()
        {
            var consoleLogging = new ConsoleLogging();
            _consoleRedirector = new ConsoleRedirector();
            _consoleRedirector.Logging = consoleLogging;
            _consoleTraceListener = new ConsoleTraceListener();
            _consoleTraceListener.Logging = consoleLogging;

            // Set console outputs (this will redirect Console.Write* output to the cryengine console)
            Console.SetOut(_consoleRedirector);
            Console.SetError(_consoleRedirector);

            // Add our trace listener (this will redirect Trace.* output to the cryengine console)
            Trace.Listeners.Add(_consoleTraceListener);
        }

        private void UnhandledExceptionOccurred(object sender, UnhandledExceptionEventArgs e)
        {
            Console.Error.WriteLine("An unhandled managed exception occured: {0}{1}", Environment.NewLine, e.ExceptionObject.ToString());
        }

        private void InitializeGameDomain()
        {
            AppDomain domain = AppDomain.CreateDomain("cemono Game");

            GameLoader gameLoader = (GameLoader)domain.CreateInstanceFromAndUnwrap(Assembly.GetExecutingAssembly().Location, typeof(GameLoader).ToString());
            gameLoader.ConsoleRedirector = _consoleRedirector;
            gameLoader.CompileAndLoad("", @"E:\Games\Crysis Wars\Mods\cemono\Game\Logic");

            _gameDomain = domain;
        }
        #endregion
    }
}
