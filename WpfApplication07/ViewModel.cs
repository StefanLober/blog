using System;
using System.ComponentModel;
using System.Windows.Input;

namespace Local
{
    public class ActionCommand : ICommand
    {
        readonly Action _execute;

        public ActionCommand(Action execute)
        {
            _execute = execute ?? throw new ArgumentNullException("execute");
        }

        public void Execute(object param) { _execute(); }

        public bool CanExecute(object parameters) { return true; }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        } 
    }

    public class ViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                var e = new PropertyChangedEventArgs(propertyName);
                PropertyChanged(this, e);
            }
        }
        
        private ICommand _changeCommand;

        public ICommand ChangeCommand => _changeCommand;

        private DisplayValueEnum _displayValue = DisplayValueEnum.Undecided;

        public ViewModel()
        {
            _changeCommand = new ActionCommand(Change);
        }

        public DisplayValueEnum DisplayValue
        {
            get { return _displayValue; }
            set
            {
                if (value != _displayValue)
                {
                    _displayValue = value;
                    OnPropertyChanged(nameof(DisplayValue));
                }
            }
        }

        private void Change() { DisplayValue = (DisplayValueEnum)(((int)DisplayValue + 1) % 3); }
    }
}