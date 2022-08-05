using System;

public class Main
{
    public Main()
    {
        Console.WriteLine("Main constructor");
    }

    public void PrintMessage()
    {
        Console.WriteLine("Hello World from C#!");
    }

    public void PrintCustomMessage(string aMessage)
    {
        Console.WriteLine($"Print from C#: {aMessage}");
    }
}