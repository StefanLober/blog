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
            if (execute == null)
                throw new ArgumentNullException("execute");

            _execute = execute;
        }

        public void Execute(object param) { _execute(); }

        public bool CanExecute(object parameters) { return true; }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        } 
    }

    public enum DisplayValueEnum : int
    {
        Undecided = 0,
        Bad = 1,
        Good = 2,
    }

    public class ViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                var e = new PropertyChangedEventArgs(propertyName);
                this.PropertyChanged(this, e);
            }
        }
        
        private ICommand _changeCommand;

        public ICommand ChangeCommand
        {
            get
            {
                if (_changeCommand == null)
                {
                    _changeCommand = new ActionCommand(Change);
                }

                return _changeCommand;
            }
        }

        private DisplayValueEnum _displayValue = DisplayValueEnum.Undecided;

        public DisplayValueEnum DisplayValue
        {
            get { return _displayValue; }
            set
            {
                if (value != _displayValue)
                {
                    _displayValue = value;
                    OnPropertyChanged("DisplayValue");
                }
            }
        }

        private void Change() { DisplayValue = (DisplayValueEnum)(((int)DisplayValue + 1) % 3); }
    }
}