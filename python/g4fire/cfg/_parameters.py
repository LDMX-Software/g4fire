"""A base parameters class that automatically does type
and existence checking

the parameters and their types are defined by the dictionary
passed to the __init__"""

class Parameters :
    """Python configuration class to help validate existence and type

    The members of this class and its children are dynamically defined
    using the key-word arguments to the constructor. Then any later attempts
    to set its members (aka attributes) will fail if the member does not
    exist or the new value is the wrong type.

    Parameters
    ----------
    kwargs : dict
        Parameters and their default values
    """

    def __init__(self, class_name, **kwargs) :
        # explicitly use super here to avoid calling our customized __setattr__
        kwargs['class_name'] = class_name
        super().__setattr__('__dict__',kwargs)

    def __setattr__(self, name, value) :
        """Customize attribute setting mechanism

        Parameters
        ----------
        name : str
            Name of member attempting to be set (i.e. after the `.`)
        value
            new value for member (i.e. stuff after `=`)

        Raises
        ------
        AttributeError : if 'name' does not exist in the members yet
        AttributeError : if 'value' is not the same type as the member
        """

        if name in self.__dict__ :
            if self.__dict__[name] is None or isinstance(value,type(self.__dict__[name])) :
                # default value was None or they are the same instance
                self.__dict__[name] = value
            else :
                raise AttributeError(f'\'{self.__class__.__name__}\' parameter \'{name}\' is of type {type(self.__dict__[name])} and not {type(value)}')
        else :
            raise AttributeError(f'\'{self.__class__.__name__}\' does not have a parameter named \'{name}\'')

    def __str__(self) :
        """Stringify this set of parameters

        Returns
        -------
        str
            A human-readable printing of the parameters in this set
        """

        s = f'{repr(self)} :\n'
        for k, v in self.__dict__.items() :
            s += f'  {k} : {v}\n'
        return s

    def __repr__(self) :
        """A shorter string representation of this Parameters set

        Returns
        -------
        str
            printing its Python class and C++ class
        """

        return f'{self.__class__.__name__} ({self.class_name})'
